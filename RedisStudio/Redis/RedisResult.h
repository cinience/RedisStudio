#pragma once

#include <vector>
#include <string>


class RedisResult
{
public:
    const string NullString;
    typedef std::vector<std::string> TSeqResultName;
    typedef std::vector<std::string> TSeqResultColumn;
    typedef std::vector< TSeqResultColumn > TSeqResultTable;

public:
    RedisResult(void);

    ~RedisResult(void);

    void Clear();
    
    void NewColumn(const string& name);

    void NewRow();

    std::size_t ColumnSize();

    std::size_t RowSize();

    const std::string& ColumnName(std::size_t idx);

    std::string& Value(std::size_t rowidx, std::size_t colidx);

private:
    TSeqResultName  m_ResultName;
    TSeqResultTable m_ResultTable;    
};
