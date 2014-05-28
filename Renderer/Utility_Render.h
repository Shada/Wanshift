#include <DXGI.h>
#include <map>

static int SizeOfFormat( DXGI_FORMAT _format )
{
	switch(_format)
	{
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
		case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
            return 128;
               
        case DXGI_FORMAT_R32G32B32_TYPELESS:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32_UINT:
        case DXGI_FORMAT_R32G32B32_SINT:
            return 96;
               
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R32G32_TYPELESS:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
	    case DXGI_FORMAT_R1_UNORM:
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
            return 64;
       
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16G16_TYPELESS:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
	    case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:			
			return 32;
               
        case DXGI_FORMAT_R8G8_TYPELESS:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
            return 16;
               
        case DXGI_FORMAT_R8_TYPELESS:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_A8_UNORM:
            return 8;

        case DXGI_FORMAT_UNKNOWN:
        default:
            return -1;
	}
}

static inline float getRTVMemSize(ID3D11RenderTargetView* _rtv)
{
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	_rtv->GetDesc(&desc);
	UINT num, elemSize;
	num		= desc.Buffer.NumElements;
	elemSize= desc.Buffer.ElementWidth;

	return float(num * elemSize);
}

static inline float getBufferMemSize(ID3D11Buffer* _buffer)
{
	float bSize;
	D3D11_BUFFER_DESC desc;

	_buffer->GetDesc(&desc);

	bSize	= (float)desc.ByteWidth;
	
	//printf("\n--- Buffer ---\n");
	//printf("buffer size(B):%f\n",bSize);

	return bSize;
}

static inline float getTextureMemSize(ID3D11Texture3D* _texture)
{
	D3D11_TEXTURE3D_DESC desc;
	_texture->GetDesc(&desc);

	float tSize, theight, twidth, tdepth, telemSize;
	theight		= (float)desc.Height;
	twidth		= (float)desc.Width;
	tdepth		= (float)desc.Depth;
	telemSize	= SizeOfFormat(desc.Format) / 8.f;	// bit to byte
	tSize		= telemSize * theight * twidth * tdepth;

	//printf("\nelement size(B):%f\nwidth:%f | height:%f | depth:%f\n", telemSize, twidth, theight, tdepth);
	//printf("file size(MB):%f\n",tSize / 1024.0f / 1024.0f);

	return tSize;
}

static inline float getTextureMemSize(ID3D11Texture2D* _texture)
{
	D3D11_TEXTURE2D_DESC desc;
	_texture->GetDesc(&desc);

	float tSize, theight, twidth, tdepth, telemSize;
	theight		= (float)desc.Height;
	twidth		= (float)desc.Width;
	tdepth		= (float)desc.ArraySize;
	telemSize	= SizeOfFormat(desc.Format) / 8.f;
	tSize		= telemSize * theight * twidth * tdepth;

	//printf("\nelement size(B):%f\nwidth:%f | height:%f | depth:%f\n", telemSize, twidth, theight, tdepth);
	//printf("file size(MB):%f\n",tSize / 1024.0f / 1024.0f);
	return tSize;
}

template<typename T>
static T addAll(std::map<int,T>& _data)
{
	T size = 0;
	//int tIndex;
	float tSize;
	for(auto t : _data)
	{

		tSize  = (T)t.second;
		//printf("index:%i | size:%f MB\n",tIndex,tSize / 1024.0f / 1024.0f);
		size += t.second;
	}

	return size;
}
