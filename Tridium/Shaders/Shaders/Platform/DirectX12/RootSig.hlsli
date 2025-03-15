#pragma once
// Tridium Default Root Signature
// The root signature will affect shader compilation for DirectX 12.
#define TRIDIUM_ENGINE_DEFAULT_ROOTSIG \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "RootConstants(num32BitConstants=12, b999), " \
    "DescriptorTable(SRV(t0, numDescriptors=1), visibility=SHADER_VISIBILITY_PIXEL), " \
