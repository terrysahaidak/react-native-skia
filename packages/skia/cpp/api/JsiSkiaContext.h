#pragma once

#include <memory>
#include <numeric>
#include <utility>
#include <vector>

#include "JsiSkHostObjects.h"
#include "RNSkLog.h"
#include <jsi/jsi.h>

#include "JsiSkPaint.h"
#include "JsiSkPoint.h"
#include "JsiSkRect.h"
#include "JsiSkTypeface.h"

#include "SkiaContext.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"

#include "include/core/SkFont.h"
#include "include/core/SkFontMetrics.h"
#include "include/core/SkColorType.h"
#include "include/gpu/graphite/dawn/DawnTypes.h"
#include "include/gpu/graphite/dawn/DawnUtils.h"
#include "include/gpu/graphite/Context.h"
#include "include/gpu/graphite/Recorder.h"
#include "include/gpu/graphite/Recording.h"
#include "include/gpu/graphite/dawn/DawnBackendContext.h"
#include "include/gpu/graphite/BackendTexture.h"
#include "include/gpu/graphite/Surface.h"

#include <GPUDevice.h>
#include <GPU.h>
#include "Convertors.h"
#include "GPUCanvasContext.h"
#include <RNFJSIConverter.h>


#pragma clang diagnostic pop

namespace RNSkia {

namespace jsi = facebook::jsi;

class JsiSkiaContext : public JsiSkWrappingSharedPtrHostObject<SkiaContext> {
public:
  EXPORT_JSI_API_TYPENAME(JsiSkiaContext, SkiaContext)

  JSI_HOST_FUNCTION(createGraphite) {
    auto gpuContext = margelo::JSIConverter<std::shared_ptr<rnwgpu::GPUCanvasContext>>::fromJSI(runtime, arguments[0], false);
    auto gpu = margelo::JSIConverter<std::shared_ptr<rnwgpu::GPU>>::fromJSI(
            runtime, arguments[1], false);
    auto device = margelo::JSIConverter<std::shared_ptr<rnwgpu::GPUDevice>>::fromJSI(
            runtime, arguments[2], false);

    auto queue = device->getQueue();

     skgpu::graphite::DawnBackendContext dawnContext;

     dawnContext.fInstance = gpu->get();
     dawnContext.fDevice = device->get();
     dawnContext.fQueue = queue->get();

     skgpu::graphite::ContextOptions options;

      // Create a Skia Graphite context for Dawn
    std::shared_ptr<skgpu::graphite::Context> skiaContext = skgpu::graphite::ContextFactory::MakeDawn(dawnContext, options);


    if (!skiaContext) {
        printf("Failed to create Skia Graphite Context with Dawn\n");
        return -1;
    }

    std::unique_ptr<skgpu::graphite::Recorder> recorder = skiaContext->makeRecorder();

    auto texture = gpuContext->getCurrentTexture();

    skgpu::graphite::DawnTextureInfo info(/*sampleCount=*/1,
                                          skgpu::Mipmapped::kNo,
                                          texture->get().GetFormat(),
                                          texture->get().GetUsage(),
                                          wgpu::TextureAspect::All);

    auto backendTex = skgpu::graphite::BackendTextures::MakeDawn(texture->get().Get());

    SkSurfaceProps props(0, kUnknown_SkPixelGeometry);

//     Create Skia Graphite surface
    auto surface = SkSurfaces::WrapBackendTexture(recorder.get(),
                                                  backendTex,
                                                  kBGRA_8888_SkColorType,
                                                  nullptr,
                                                  &props);
    if (!surface) {
        printf("Failed to create Skia Surface with Graphite\n");
        return -1;
    }

    SkCanvas* canvas = surface->getCanvas();

    if (canvas) {
        // Clear the canvas with a color
        canvas->clear(SK_ColorWHITE);

        // Draw a simple rectangle
        SkPaint paint;
        paint.setColor(SK_ColorRED);
        canvas->drawRect(SkRect::MakeWH(100, 100), paint);
    }

    auto recording = recorder->snap();
    if (recording) {
     skiaContext->insertRecording({recording.get()});
    }

    skiaContext->submit();

    gpuContext->present();


    return jsi::Value::undefined();
  }

  JSI_HOST_FUNCTION(getSurface) {
    auto surface = getObject()->getSurface();
    return jsi::Object::createFromHostObject(
        runtime,
        std::make_shared<JsiSkSurface>(getContext(), std::move(surface)));
  }

  JSI_HOST_FUNCTION(present) {
    getObject()->present();
    return jsi::Value::undefined();
  }

  JSI_EXPORT_FUNCTIONS(JSI_EXPORT_FUNC(JsiSkiaContext, getSurface),
                       JSI_EXPORT_FUNC(JsiSkiaContext, createGraphite),
                       JSI_EXPORT_FUNC(JsiSkiaContext, present))

  JsiSkiaContext(std::shared_ptr<RNSkPlatformContext> context,
                 std::shared_ptr<SkiaContext> ctx)
      : JsiSkWrappingSharedPtrHostObject(std::move(context), std::move(ctx)) {}

  /**
   * Creates the function for construction a new instance of the SkFont
   * wrapper
   * @param context Platform context
   * @return A function for creating a new host object wrapper for the SkFont
   * class
   */
  static const jsi::HostFunctionType
  createCtor(std::shared_ptr<RNSkPlatformContext> context) {
    return JSI_HOST_FUNCTION_LAMBDA {
      jsi::BigInt pointer = arguments[0].asBigInt(runtime);
      const uintptr_t nativeBufferPointer = pointer.asUint64(runtime);
      void *surface = reinterpret_cast<void *>(nativeBufferPointer);
      auto width = static_cast<int>(arguments[1].asNumber());
      auto height = static_cast<int>(arguments[2].asNumber());
      
      auto result =
          context->makeContextFromNativeSurface(surface, width, height);
      // Return the newly constructed object
      return jsi::Object::createFromHostObject(
          runtime, std::make_shared<JsiSkiaContext>(std::move(context),
                                                    std::move(result)));
    };
  }
};

} // namespace RNSkia
