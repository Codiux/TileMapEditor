#include "Tily.h"

#include <TGUI/TGUI.hpp>

namespace ty
{
	sf::Uint16 Settings::TileSize = 64;
	sf::Uint16 Settings::ChunkWidth = 16;
	sf::Uint16 Settings::ChunkHeight = 16;
	sf::Uint16 Settings::TextureSize = 512;



	Chunk::Chunk()
	{
		m_vertices = nullptr;
		m_ids = nullptr;

		// create vertex buffer
		m_vBuffer.setPrimitiveType(sf::PrimitiveType::Quads);
		m_vBuffer.setUsage(sf::VertexBuffer::Usage::Dynamic);
		m_vBuffer.create(Settings::ChunkWidth*Settings::ChunkHeight * 4);
		
		// cpu side vertex data
		m_vertices = new sf::Vertex[m_vBuffer.getVertexCount()];

		// create tile ID array
		m_ids = new sf::Uint16*[Settings::ChunkHeight];
		for (sf::Uint16 y = 0; y < Settings::ChunkHeight; y++)
			m_ids[y] = new sf::Uint16[Settings::ChunkWidth];

		// initialize vertex array
		for (int x = 0; x < ty::Settings::ChunkWidth; x++)
			for (int y = 0; y < ty::Settings::ChunkHeight; y++) {
				m_ids[y][x] = 0;

				sf::Vertex* tile = &m_vertices[(x + y * Settings::ChunkWidth) * 4];

				tile[0].position = sf::Vector2f(x*Settings::TileSize, y*Settings::TileSize);
				tile[1].position = sf::Vector2f((x + 1)*Settings::TileSize, y*Settings::TileSize);
				tile[2].position = sf::Vector2f((x + 1)*Settings::TileSize, (y + 1)*Settings::TileSize);
				tile[3].position = sf::Vector2f(x*Settings::TileSize, (y + 1)*Settings::TileSize);

				tile[0].texCoords = sf::Vector2f(0, 0);
				tile[1].texCoords = sf::Vector2f(Settings::TileSize, 0);
				tile[2].texCoords = sf::Vector2f(Settings::TileSize, Settings::TileSize);
				tile[3].texCoords = sf::Vector2f(0, Settings::TileSize);
			}

		// send the data to the gpu - vertex buffer
		m_vBuffer.update(m_vertices);
	}
	Chunk::~Chunk()
	{
		// free memory
		if (m_vertices != nullptr)
			delete[] m_vertices;

		if (m_ids != nullptr) {
			for (sf::Uint16 y = 0; y < Settings::ChunkHeight; y++)
				delete[] m_ids[y];
			delete[] m_ids;
		}
	}
	void Chunk::Set(sf::Uint32 x, sf::Uint32 y, sf::Uint16 id)
	{
		sf::Uint32 offset = (x + y * Settings::ChunkWidth) * 4;

		m_ids[y][x] = id;

		sf::Vertex* tile = &m_vertices[offset];

		// calculate and update texture coords
		sf::Uint16 tx = (id*Settings::TileSize) % Settings::TextureSize;
		sf::Uint16 ty = ((id*Settings::TileSize) / Settings::TextureSize)*Settings::TileSize;

		tile[0].texCoords = sf::Vector2f(tx, ty);
		tile[1].texCoords = sf::Vector2f(tx + Settings::TileSize, ty);
		tile[2].texCoords = sf::Vector2f(tx + Settings::TileSize, ty + Settings::TileSize);
		tile[3].texCoords = sf::Vector2f(tx, ty + Settings::TileSize);

		// update the vertex buffer, send only necessary info
		m_vBuffer.update(m_vertices+offset, 4, offset);
	}
	void Chunk::draw(sf::RenderTarget & target, sf::RenderStates states) const
	{
		// apply the transform
		states.transform *= getTransform();

		// draw the vertex array
		target.draw(m_vBuffer, states);
	}



	Map::Map()
	{
		m_chunks = nullptr;
	}
	Map::~Map()
	{
		if (m_chunks != nullptr) {
			for (sf::Uint8 z = 0; z < m_layers; z++) {
				for (sf::Uint32 y = 0; y < m_chunkVCount; y++)
					delete[] m_chunks[z][y];
				delete[] m_chunks[z];
			}
			delete[] m_chunks;
		}
	}
	void Map::Create(sf::Uint32 width, sf::Uint32 height, sf::Uint8 layers)
	{
		m_width = width;
		m_height = height;
		m_layers = layers;

		m_chunkHCount = ((width - 1) / Settings::ChunkWidth) + 1;
		m_chunkVCount = ((height - 1) / Settings::ChunkHeight) + 1;

		m_chunks = new Chunk**[layers];

		for (sf::Uint8 z = 0; z < layers; z++) {
			m_chunks[z] = new Chunk*[m_chunkVCount];
			for (sf::Uint32 y = 0; y < m_chunkVCount; y++) {
				m_chunks[z][y] = new Chunk[m_chunkHCount];
				for (sf::Uint32 x = 0; x < m_chunkHCount; x++)
					m_chunks[z][y][x].setPosition(x*Settings::ChunkWidth*Settings::TileSize, y*Settings::ChunkHeight*Settings::TileSize);
			}
		}
	}
	void Map::Set(sf::Uint32 x, sf::Uint32 y, sf::Uint8 layer, sf::Uint16 id)
	{
		if (x >= m_width || y >= m_height || x < 0 || y < 0)
			return;

		sf::Uint32 cx = x / Settings::ChunkWidth;
		sf::Uint32 cy = y / Settings::ChunkHeight;

		m_chunks[layer][cy][cx].Set(x%Settings::ChunkWidth, y%Settings::ChunkHeight, id);
	}
	int Map::Get(sf::Uint32 x, sf::Uint32 y, sf::Uint8 layer)
	{
		if (x >= m_width || y >= m_height || x < 0 || y < 0)
			return -1;

		sf::Uint32 cx = x / Settings::ChunkWidth;
		sf::Uint32 cy = y / Settings::ChunkHeight;

		return m_chunks[layer][cy][cx].Get(x%Settings::ChunkWidth, y%Settings::ChunkHeight);
	}
	void Map::DrawLayer(sf::Uint8 z, tgui::Canvas::Ptr target, sf::RenderStates rs) const
	{
		sf::View view = target->getView();
		sf::IntRect vr(view.getCenter().x - (view.getSize().x / 2), view.getCenter().y - (view.getSize().y / 2), view.getSize().x, view.getSize().y);

		// calculate the region of chunks that are in the view
		int csY = std::max(0, (vr.top / Settings::TileSize) / Settings::ChunkHeight);
		int ceY = std::min((int)m_chunkVCount - 1, csY + 1 + ((vr.height / Settings::TileSize) / Settings::ChunkHeight));
		int csX = std::max(0, (vr.left / Settings::TileSize) / Settings::ChunkWidth);
		int ceX = std::min((int)m_chunkHCount - 1, csX + 1 + ((vr.width / Settings::TileSize) / Settings::ChunkWidth));

		rs.transform *= getTransform();

		// render chunks
		for (sf::Uint32 y = csY; y <= ceY; y++)
			for (sf::Uint32 x = csX; x <= ceX; x++)
				target->draw(m_chunks[z][y][x], rs);
	}

	void Map::DrawLayer(sf::Uint8 z, sf::RenderTarget & target, sf::RenderStates rs) const
	{
		sf::View view = target.getView();
		sf::IntRect vr(view.getCenter().x - (view.getSize().x / 2), view.getCenter().y - (view.getSize().y / 2), view.getSize().x, view.getSize().y);

		// calculate the region of chunks that are in the view
		int csY = std::max(0, (vr.top / Settings::TileSize) / Settings::ChunkHeight);
		int ceY = std::min((int)m_chunkVCount - 1, csY + 1 + ((vr.height / Settings::TileSize) / Settings::ChunkHeight));
		int csX = std::max(0, (vr.left / Settings::TileSize) / Settings::ChunkWidth);
		int ceX = std::min((int)m_chunkHCount - 1, csX + 1 + ((vr.width / Settings::TileSize) / Settings::ChunkWidth));

		rs.transform *= getTransform();

		// render chunks
		for (sf::Uint32 y = csY; y <= ceY; y++)
			for (sf::Uint32 x = csX; x <= ceX; x++)
				target.draw(m_chunks[z][y][x], rs);
	}
}
