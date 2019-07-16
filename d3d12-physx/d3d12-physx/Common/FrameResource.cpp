#include "FrameResource.h"

std::unique_ptr<FrameResource<PassConstants>> gPassCB = std::make_unique<FrameResource<PassConstants>>();