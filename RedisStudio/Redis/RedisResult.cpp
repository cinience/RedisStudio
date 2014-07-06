#include "stdafx.h"
#include "RedisResult.h"


RedisResult::RedisResult(void)
{
}

RedisResult::~RedisResult(void)
{
}


void RedisResult::Clear()
{
    m_ResultTable.clear();
    m_ResultName.clear();
}

void RedisResult::NewColumn( const string& name )
{
    m_ResultName.push_back(name);
    if (ColumnSize() > 0)
    {
        m_ResultTable.resize(ColumnSize());
        m_ResultTable[ColumnSize()-1].resize(RowSize());
    }
    else 
    {
        m_ResultTable.resize(ColumnSize());
    }
}

void RedisResult::NewRow()
{
    TSeqResultTable::iterator it = m_ResultTable.begin();
    TSeqResultTable::iterator itend = m_ResultTable.end();

    for (; it!=itend; ++it)
    {
        it->push_back(NullString);
    }
}

std::size_t RedisResult::ColumnSize()
{
    return m_ResultName.size();
}

std::size_t RedisResult::RowSize()
{
    if (!m_ResultName.empty())
        return m_ResultTable[0].size();

    return 0;
}

const std::string& RedisResult::ColumnName( std::size_t idx )
{
    if (idx < m_ResultName.size())
    {
        return m_ResultName[idx];            
    }
    return NullString;
}

std::string& RedisResult::Value( std::size_t rowidx, std::size_t colidx )
{
    return m_ResultTable[colidx][rowidx];
}
