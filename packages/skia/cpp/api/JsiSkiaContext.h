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
#include "include/gpu/graphite/Context.h"
#include "include/gpu/graphite/Recorder.h"
#include "include/gpu/graphite/Recording.h"
#include "include/gpu/graphite/Surface.h"
#include "include/gpu/graphite/dawn/DawnBackendContext.h"
#include <GPUAdapter.h>

#pragma clang diagnostic pop

namespace RNSkia {

namespace jsi = facebook::jsi;

class JsiSkiaContext : public JsiSkWrappingSharedPtrHostObject<SkiaContext> {
public:
  EXPORT_JSI_API_TYPENAME(JsiSkiaContext, SkiaContext)

  JSI_HOST_FUNCTION(createGraphite) {
    return jsi::undefined();
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


      
      skgpu::graphite::DawnBackendContext dawnContext;
      
      
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
