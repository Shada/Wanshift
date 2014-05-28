#include "RenderDX11.h"

//////////////////////
//		DRAW		//
//////////////////////

void RenderDX11::draw(UINT _vertexCount, UINT _startVertexLocation)
{
	deviceContext->Draw(_vertexCount, _startVertexLocation);
}

void RenderDX11::drawIndexed(UINT _indexCount, UINT _startIndexLocation, UINT _baseVertexLocation)
{
	deviceContext->DrawIndexed(_indexCount, _startIndexLocation, _baseVertexLocation);
}

void RenderDX11::drawInstanced(UINT _vertexCount,UINT _instanceCount, UINT _startVertexLocation, UINT _startInstanceLocation)
{
	deviceContext->DrawInstanced(_vertexCount,_instanceCount,_startVertexLocation,_startInstanceLocation);
}

void RenderDX11::drawAuto()
{
	deviceContext->DrawAuto();
}

void RenderDX11::presentSwapChain()
{
	swapChain->Present(0, 0);
}

void RenderDX11::clearRenderTargets(glm::vec4 _clearColor)
{
	for (UINT i = 0; i < sharedContainer->getRenderTargetSize(); i++)
		deviceContext->ClearRenderTargetView(sharedContainer->getRenderTargetView(i), &_clearColor[0]);
	for(UINT i = 0; i < sharedContainer->getDepthStensilViewSize(); i++)
		deviceContext->ClearDepthStencilView(sharedContainer->getDepthStencilView(i), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void RenderDX11::clearRenderTargetSingle(UINT _renderTargetIndex, glm::vec4 _clearColor)
{
	deviceContext->ClearRenderTargetView(sharedContainer->getRenderTargetView(_renderTargetIndex), &_clearColor[0]);
}

void RenderDX11::clearDepthStencil(UINT _depthStencilIndex)
{
	ID3D11DepthStencilView* dsv = sharedContainer->getDepthStencilView(_depthStencilIndex);

	deviceContext->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void RenderDX11::presentFinalScene()
{
	//clearDepthStencil(0);
	setRasterState(2);
	setRenderTargetViews( backBufferRenderTarget, 1, -1 );

	setNullVertexBuffers();

	setSamplers();	
	setBlendState(1);
	
	/* trilist topology */
	deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	/* push final shaders */
	setShaders( finalSceneShaderVS, -1, -1, -1, finalSceneShaderPS );
	
	/* push all rendertarget textures */
 	setPixelShaderResource(10, getHandleIndex("fakeBuffer",			ResourceType::ShaderResourceView),1);
	setPixelShaderResource(11, getHandleIndex("postEffectTexture",	ResourceType::ShaderResourceView),1);
	//setPixelShaderResource(12,  getHandleIndex("skyTexture",		ResourceType::ShaderResourceView),1);

	

	/* set vertex id layout*/
	setInputLayout((int)LayoutTypes::LAYOUT_FINAL);

	/* draw quad by vertex id */
	deviceContext->Draw( 6, 0 );

	setPixelShaderResource(10,-1,1);
	setPixelShaderResource(11, -1,1);
}