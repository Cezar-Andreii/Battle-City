#pragma once
#include "TileType.h"
#include<vector>
#include<array>
#include<cstdint>
#include<iostream>
#include<random>

class Map
{
	private:
		std::vector<std::vector<TileType>>m_gameArea;
		size_t m_height;
		size_t m_width;
		void Allocation();
		std::array<std::pair<size_t, size_t>,4>m_startPositions;
		void Explode(const std::pair<size_t, size_t>& t_position);

	public:
		size_t getHeight() const;
		size_t getWidth() const;

		void GenerateMap();
		void SetStartPositions();
		void Draw() const;

		TileType GetTile(const std::pair<size_t, size_t>&t_position) const;
		void DestroyTile(const std::pair<size_t, size_t>&t_position);
		void SetTile(const std::pair<size_t, size_t>& t_position,const TileType& t_tile);
		
};