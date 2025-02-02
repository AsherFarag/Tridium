#include "tripch.h"
#include "RHIGlobals.h"

// For declaring RHIVertexElementTypes
// I didn't feel like making a new file for this
#include "RHIDefinitions.h"

namespace Tridium {

	RHIGlobals s_RHIGlobals;

	uint32_t s_GPUFrameTime = 0u;

	const RHIVertexElementType RHIVertexElementType::Unknown = { ERHIDataType::Unknown, 0 };
	const RHIVertexElementType RHIVertexElementType::Half = { ERHIDataType::Float16, 1 };
	const RHIVertexElementType RHIVertexElementType::Half2 = { ERHIDataType::Float16, 2 };
	const RHIVertexElementType RHIVertexElementType::Half3 = { ERHIDataType::Float16, 3 };
	const RHIVertexElementType RHIVertexElementType::Half4 = { ERHIDataType::Float16, 4 };
	const RHIVertexElementType RHIVertexElementType::Float = { ERHIDataType::Float32, 1 };
	const RHIVertexElementType RHIVertexElementType::Float2 = { ERHIDataType::Float32, 2 };
	const RHIVertexElementType RHIVertexElementType::Float3 = { ERHIDataType::Float32, 3 };
	const RHIVertexElementType RHIVertexElementType::Float4 = { ERHIDataType::Float32, 4 };
	const RHIVertexElementType RHIVertexElementType::Int = { ERHIDataType::Int32, 1 };
	const RHIVertexElementType RHIVertexElementType::Int2 = { ERHIDataType::Int32, 2 };
	const RHIVertexElementType RHIVertexElementType::Int3 = { ERHIDataType::Int32, 3 };
	const RHIVertexElementType RHIVertexElementType::Int4 = { ERHIDataType::Int32, 4 };
	const RHIVertexElementType RHIVertexElementType::UInt = { ERHIDataType::UInt32, 1 };
	const RHIVertexElementType RHIVertexElementType::UInt2 = { ERHIDataType::UInt32, 2 };
	const RHIVertexElementType RHIVertexElementType::UInt3 = { ERHIDataType::UInt32, 3 };
	const RHIVertexElementType RHIVertexElementType::UInt4 = { ERHIDataType::UInt32, 4 };
}