#pragma once
// Tridium Default Root Signature
// The root signature will affect shader compilation for DirectX 12.
#define TRIDIUM_ENGINE_DEFAULT_ROOTSIG \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "RootConstants(num32BitConstants=12, b999), " \
    "DescriptorTable(SRV(t0, numDescriptors=4), visibility=SHADER_VISIBILITY_PIXEL), " \
    "DescriptorTable(UAV(u0), visibility=SHADER_VISIBILITY_ALL), " \
    "DescriptorTable(Sampler(s0, numDescriptors=4), visibility=SHADER_VISIBILITY_PIXEL)"


#define ROOTSIG [RootSignature(TRIDIUM_ENGINE_DEFAULT_ROOTSIG)]

    
