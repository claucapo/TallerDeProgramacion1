#pragma once
class Mapeable
{
public:
	int generarCodigo();
	bool operator<(const Mapeable other);
	bool operator>(const Mapeable other);
	bool operator==(const Mapeable other);
};
