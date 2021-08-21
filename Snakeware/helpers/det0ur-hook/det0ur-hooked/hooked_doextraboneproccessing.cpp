#include "../detour_hook.h"

DetourHooks::DoExtraBoneProcessingT DetourHooks::OriginalDoExtraBoneProcessing;

void _fastcall DetourHooks::hkDoExtraBoneProcessing(void * ecx, uint32_t, studiohdr_t * hdr, Vector * pos, Quaternion * q, const matrix3x4_t & matrix, uint8_t * bone_computed, void * context) {

	return;
}