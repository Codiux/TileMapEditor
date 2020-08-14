#ifndef __TILY_H__
#define __TILY_H__

#include <vector>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <TGUI/TGUI.hpp>

namespace ty
{
	/*
		This class is only used for setting up the maps in your game.
		You can modify the chunk size, texture size (the texture must have
		same width and height) and tile size. 
	*/
	class Settings
	{
	public:
		static sf::Uint16 TileSize;

		static sf::Uint16 ChunkWidth;
		static sf::Uint16 ChunkHeight;

		static sf::Uint16 TextureSize;
	};

	/*
		A map contains a grid of chunks. And a chunk is a grid of tiles.
		Having the map split into smaller parts helps the rendering performance.
	*/
	class Chunk : public sf::Drawable, public sf::Transformable
	{
	public:
		Chunk();
		~Chunk();

		// set a tile at a given position in this chunk
		void Set(sf::Uint32 x, sf::Uint32 y, sf::Uint16 id);

		// get a tile at the given position in this chunk
		inline sf::Uint16 Get(sf::Uint32 x, sf::Uint32 y)
		{
			return m_ids[y][x];
		}

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	private:
		// vertex information
		sf::Vertex* m_vertices;
		sf::VertexBuffer m_vBuffer;

		// stores tile IDs
		sf::Uint16** m_ids;
	};


	/*
		Map is a grid of chunks. It manages all the chunks so that we dont have
		to worry about specific chunks or rendering them.
	*/
	class Map : public sf::Transformable
	{
	public:
		Map();
		~Map();

		void Create(sf::Uint32 width, sf::Uint32 height, sf::Uint8 layers);

		void Set(sf::Uint32 x, sf::Uint32 y, sf::Uint8 layer, sf::Uint16 id);
		int Get(sf::Uint32 x, sf::Uint32 y, sf::Uint8 layer);

		void DrawLayer(sf::Uint8 layer, tgui::Canvas::Ptr target, sf::RenderStates rs) const;

		void DrawLayer(sf::Uint8 layer, sf::RenderTarget & target, sf::RenderStates rs) const;

		

	private:
		Chunk*** m_chunks;
		sf::Uint32 m_width, m_height, m_chunkHCount, m_chunkVCount, m_layers;
	};
}

#endif
