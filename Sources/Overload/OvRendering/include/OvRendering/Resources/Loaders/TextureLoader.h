/**
* @project: Overload
* @author: Overload Tech.
* @restrictions: This software may not be resold, redistributed or otherwise conveyed to a third party.
*/

#pragma once

#include <string>
#include <vector>

#include "OvRendering/Resources/Texture.h"


namespace OvRendering::Resources::Loaders
{
	/**
	* Handle the Texture creation and destruction
	*/
	class API_OVRENDERING TextureLoader
	{
	public:
		/**
		* Disabled constructor
		*/
		TextureLoader() = delete;

		/**
		* Create a texture from file
		* @param p_filePath
		* @param p_firstFilter
		* @param p_secondFilter
		* @param p_generateMipmap
		*/
		static Texture* Create(const std::string& p_filepath, OvRendering::Settings::ETextureFilteringMode p_firstFilter, OvRendering::Settings::ETextureFilteringMode p_secondFilter, bool p_generateMipmap);

		/**
		* Create a texture from a single pixel color
		* @param p_data
		* @param p_firstFilder
		* @param p_secondFilter
		* @param p_generateMipmap
		*/
		static Texture* CreateColor(uint32_t p_data, OvRendering::Settings::ETextureFilteringMode p_firstFilter, OvRendering::Settings::ETextureFilteringMode p_secondFilter, bool p_generateMipmap);

		/**
		* Create a texture from memory
		* @param p_data
		* @param p_width
		* @param p_height
		* @param p_firstFilder
		* @param p_secondFilter
		* @param p_generateMipmap
		*/
		static Texture* CreateFromMemory(uint8_t* p_data, uint32_t p_width, uint32_t p_height, OvRendering::Settings::ETextureFilteringMode p_firstFilter, OvRendering::Settings::ETextureFilteringMode p_secondFilter, bool p_generateMipmap);

		/**
		* Reload a texture from file
		* @param p_texture
		* @param p_filePath
		*/
		static void Reload(Texture& p_texture, const std::string& p_filePath);

		/**
		* Destroy a texture
		* @param p_textureInstance
		*/
		static bool Destroy(Texture*& p_textureInstance);
	};
}