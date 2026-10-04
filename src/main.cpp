#include "core/application.hpp"
#include "core/gpio.hpp"
#include "config/application_config.hpp"

#include "drivers/camera.hpp"

// int main(int argc, char** argv) {
//     if (argc != 2) {
//         std::cerr << "Usage: program config/settings.json\n";

//         return -1;
//     }

//     // init raspberry gpio
//     turret::gpio::setup();

//     turret::application app(
//         turret::application_config::read(argv[1])
//     );

//     app.start();
// }

#include <libcamera/libcamera.h>

using namespace libcamera;
using namespace std::chrono_literals;

std::shared_ptr<Camera> try_acquire_camera(CameraManager& manager) {
    auto cameras = manager.cameras();

    if (cameras.empty()) {
        std::cerr << "No cameras found!\n";

        return nullptr;
    }

    // only expecting one camera for this project
    auto& camera = cameras[0];

    if (camera->acquire() < 0) {
        std::cerr << "Failed to acquire camera \n";

        return nullptr;
    }

    return camera;
}

std::unique_ptr<CameraConfiguration> try_configure_camera(std::shared_ptr<Camera>& camera) {
    std::unique_ptr<CameraConfiguration> config = camera->generateConfiguration(
        { StreamRole::Viewfinder }
    );

    if (config == nullptr) {
        std::cerr << "Failed to generate configuration\n";

        camera->release();

        return nullptr;
    }

    StreamConfiguration& stream_config = config->at(0);
    std::cout << "Default viewfinder configuration is: " << stream_config.toString() << std::endl;

    // apply config
    // stream_config.size.width = 640;
    // stream_config.size.height = 480;

    if (config->validate() == CameraConfiguration::Invalid) {
        std::cerr << "Invalid camera configuration\n";

        camera->release();

        return nullptr;
    }

    if (auto ret = camera->configure(config.get()); ret < 0) {
        std::cerr << "Failed to configure camera\n";

        camera->release();
        
        return nullptr;
    }

    return config;
}

std::unique_ptr<FrameBufferAllocator> create_allocator(std::shared_ptr<Camera>& camera, std::unique_ptr<CameraConfiguration>& config) {
    auto allocator = std::make_unique<FrameBufferAllocator>(camera); // probably dont need to heap alloc when as a member
    Stream* view_finder_stream = config->at(0).stream(); 

    if (allocator->allocate(view_finder_stream) < 0) {
        std::cerr << "Failed to allocate buffers\n";

        camera->release();

        return nullptr;
    }

    return allocator;
}

static void on_request_complete(Request* req) {
    if (req->status() == Request::RequestCancelled) return;

    for (auto& [stream, buffer] : req->buffers()) {
        std::cout
            << "  bytesused: "
            << buffer->metadata().planes()[0].bytesused
            << '\n';
    }
}

int main() {
    CameraManager cm;

    if (cm.start() < 0) {
        std::cerr << "Failed to start CameraManager\n";
        return 1;
    }

    std::shared_ptr<Camera> camera = try_acquire_camera(cm);

    if (camera == nullptr) {
        cm.stop();
        
        return 1;
    }

    std::unique_ptr<CameraConfiguration> config = try_configure_camera(camera);

    if (config == nullptr) {
        cm.stop();

        return 1;
    }

    std::unique_ptr<FrameBufferAllocator> allocator = create_allocator(camera, config);
    const auto& frame_buffers = allocator->buffers(config->at(0).stream());

    std::vector<std::unique_ptr<Request>> requests;

    for (size_t i = 0; i < frame_buffers.size(); i++) {
        std::unique_ptr<Request> req = camera->createRequest();

        const auto& frame_buffer = frame_buffers[i];

        if (auto ret = req->addBuffer(config->at(0).stream(), frame_buffer.get()); ret < 0) {
            std::cerr << "Can't set buffer for request" << std::endl;
            
            return ret;
        }

        requests.emplace_back(std::move(req));
    }

    camera->requestCompleted.connect(on_request_complete);

    camera->start();

    for (std::unique_ptr<Request> &request : requests)
        camera->queueRequest(request.get());

    std::this_thread::sleep_for(3s);
}