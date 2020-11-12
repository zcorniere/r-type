/*
** EPITECH PROJECT, 2021
** R-type
** File description:
** R-type
*/

#include <algorithm>
#include "app/network/tcpsockmngr.hpp"

network::TcpSockMngr::TcpSockMngr(Console &console, const std::string &ip, short port, std::vector<std::pair<long, bool>>) :
    console(console), socket(context), resolver(context), run_thread([this](){context.run();})
{
    // TODO
    downloadAllAssets();
}

network::TcpSockMngr::~TcpSockMngr()
{
    if (run_thread.joinable())
        run_thread.join();
}

long network::TcpSockMngr::receiveAsset()
{
    protocol::tcp::AssetPackage body;
    std::vector<std::byte> buff;
    // body type
    auto len = sizeof(body.type);
    buff.resize(len);
    auto size = socket.receive(boost::asio::buffer(buff, len));
    std::memcpy(&body.type, buff.data(), size);
    // body id_asset
    len = sizeof(body.id_asset);
    buff.resize(len);
    size = socket.receive(boost::asio::buffer(buff, len));
    std::memcpy(&body.id_asset, buff.data(), size);
    // body size_data
    len = sizeof(body.size_data);
    buff.resize(len);
    size = socket.receive(boost::asio::buffer(buff, len));
    std::memcpy(&body.size_data, buff.data(), size);
    if (body.type == protocol::tcp::AssetPackage::Type::Texture) {
        // body size_config
        len = sizeof(body.size_config);
        buff.resize(len);
        size = socket.receive(boost::asio::buffer(buff, len));
        std::memcpy(&body.size_config, buff.data(), size);
    }
    // body data
    len = body.size_data;
    buff.resize(len);
    size = socket.receive(boost::asio::buffer(buff, len));
    std::memcpy(body.data.data(), buff.data(), size);
    if (body.type == protocol::tcp::AssetPackage::Type::Texture) {
        // body config
        len = body.size_config;
        buff.resize(len);
        size = socket.receive(boost::asio::buffer(buff, len));
        std::memcpy(body.config.data(), buff.data(), size);
    }
    // body is build
    if (body.type == protocol::tcp::AssetPackage::Type::Sound) {  // Sound
        Asset asset;
        asset.type = Asset::Type::Sound;
        asset.id_asset = body.id_asset;
        asset.sound_buffer.loadFromMemory(body.data.data(), body.data.size());
        asset.sound.setBuffer(asset.sound_buffer);
        assets.push_back(asset);
    } else {    // Texture
        std::string config;
        config.resize(body.size_config);
        std::memcpy(config.data(), body.config.data(), body.size_config);
        std::istringstream is(config);
        ptree json;
        read_json(is, json);
        for (auto& v : json.get_child("sprites")) {
            if (v.first.empty()) {
                console.log("Error [TCP]: Config file is incorrect");
                break;
            }
            SpriteConfig config {};
            config.id = v.second.get<int>("id");
            config.origin_x = v.second.get<int>("origin_x");
            config.origin_y = v.second.get<int>("origin_y");
            config.width = v.second.get<int>("width");
            config.height = v.second.get<int>("height");
            // config is build
            Asset asset;
            asset.type = Asset::Type::Texture;
            asset.id_asset = body.id_asset;
            asset.texture.loadFromMemory(body.data.data(), body.data.size());
            asset.id_tile = static_cast<long>(config.id);
            asset.sprite.setTexture(asset.texture);
            asset.sprite.setTextureRect({config.origin_x, config.origin_y, config.width, config.height});
            assets.push_back(asset);
        }
    }
    return body.id_asset;
}

void network::TcpSockMngr::do_receive()
{
    socket.async_wait(tcp::socket::wait_read, [&](const boost::system::error_code &error) {
        if (error || socket.available() < sizeof(protocol::MessageHeader<UdpCode>))
            do_receive();
        // Get the header
        protocol::MessageHeader<TcpCode> header;
        auto len = sizeof(header);
        std::vector<std::byte> buff;
        buff.resize(len);
        auto size = socket.receive(boost::asio::buffer(buff, len));
        buff.resize(size);
        std::memcpy(&header, buff.data(), size);
        // check the header
        if (header.firstbyte != protocol::magic_number.first || header.secondbyte != protocol::magic_number.second)
            do_receive();
        if (header.code != TcpCode::AssetPackage) {
            console.log("Error [TCP]: Server sent wrong data");
            do_receive();
        }
        // get the body & work on it
        auto asset_id = receiveAsset();
        // update my assets_ids_list
        auto it = std::find_if(assets_ids_list.begin(), assets_ids_list.end(), [&](auto &asset_id_item){
            return asset_id_item.first == asset_id;
        });
        if (it == assets_ids_list.end()) {
            console.log("Error [TCP]: Downloaded Asset is not present in assets_ids_list");
            return;
        }
        it->second = true;
        downloadAllAssets();
    });
}

void network::TcpSockMngr::do_send(protocol::MessageToSend<TcpCode> message)
{
    std::size_t length = sizeof(message.head) + message.head.body_size;
    std::vector<std::byte> buffer;
    buffer.resize(length);
    std::memcpy(buffer.data(), &message.head, sizeof(message.head));
    std::memcpy(buffer.data() + sizeof(message.head), message.body.data(), message.head.body_size);
    boost::asio::async_write(socket, boost::asio::buffer(buffer, length),
        [this](boost::system::error_code ec, std::size_t) {
                if (ec) {
                    console.log("Error [TCP]: Sending failed");
                } else {
                    do_receive();
                }
            });
}

void network::TcpSockMngr::downloadAsset(long asset_id) {
    protocol::MessageToSend<TcpCode> message;
    message.head.code = TcpCode::AssetAsk;
    protocol::tcp::AssetAsk asset_ask {asset_id};
    message.head.body_size = sizeof(asset_ask);
    std::memcpy(message.body.data(), &asset_ask, sizeof(asset_ask));
    do_send(message);
}

void network::TcpSockMngr::downloadAllAssets()
{
    auto it = std::find_if(assets_ids_list.begin(), assets_ids_list.end(), [](const auto &asset_id){
        return !asset_id.second;
    });
    if (it == assets_ids_list.end()) {
        console.log("Success [TCP]: All downloads are completed");
        is_download_finish = true;
    } else {
        console.log("[TCP]: Downloading asset " + std::to_string(it->first));
        downloadAsset(it->first);
    }
}

bool network::TcpSockMngr::isDownloadFinished() const
{
    if (assets_ids_list.empty()) {
        console.log("Error [TCP]: Download: assets_ids_list is empty");
        return false;
    }
    return is_download_finish;
}

std::vector<Asset> network::TcpSockMngr::getAssets() const
{
    return assets;
}


