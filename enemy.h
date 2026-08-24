#include <compare>

struct Handle
{
	unsigned int nIndex;
	unsigned int nGeneration;

	auto operator<=>(const Handle &) const = default;
};

class CGrok
{
public:
	Handle m_myHandle;
};

class CManage
{
	CGrok *GetGrok(const Handle &handle);

	CGrok *m_pGrok;
};

CGrok *CManage::GetGrok(const Handle &handle)
{
	if (m_pGrok->m_myHandle != handle)
	{
		return nullptr;
	}

	return m_pGrok;
}