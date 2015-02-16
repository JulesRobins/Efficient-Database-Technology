//
//  Database.h
//  Project 4
//
//  Created by Jules Robins on 3/9/14.
//  Copyright (c) 2014 Jules Robins. All rights reserved.
//

#ifndef __Project_4__Database__
#define __Project_4__Database__

#include <string>
#include <vector>
#include "MultiMap.h"

class	Database
{
public:
    enum	IndexType	{	it_none,	it_indexed	};
    enum	OrderingType	{	ot_ascending,	ot_descending	};
	
    struct	FieldDescriptor
    {
        std::string	name;
        IndexType	index;
    };
	
    struct	SearchCriterion
    {
        std::string	fieldName;
        std::string	minValue;
        std::string	maxValue;
    };
	
    struct	SortCriterion
    {
        std::string	fieldName;
        OrderingType	ordering;
    };
    
    static	const	int	ERROR_RESULT	=	-1;
	
    Database();
    ~Database();
    bool	specifySchema(const	std::vector<FieldDescriptor>&	schema);
    bool	addRow(const	std::vector<std::string>&	rowOfData);
    bool	loadFromURL(std::string	url);
    bool	loadFromFile(std::string	filename);
    int	getNumRows()	const;
    bool	getRow(int	rowNum,	std::vector<std::string>&	row)	const;
    int	search(const	std::vector<SearchCriterion>&	searchCriteria,
               const	std::vector<SortCriterion>&	sortCriteria,
               std::vector<int>&	results);
	
private:
    //	To	prevent	Databases	from	being	copied	or	assigned,	declare	these	members
    //	private	and	do	not	implement	them.
    Database(const	Database&	other);
    Database&	operator=(const	Database&	rhs);
    std::vector<std::vector<std::string>> m_rows;
    std::vector<MultiMap*> m_fieldIndex;
    std::vector<FieldDescriptor> m_schema;
    std::vector<int> m_fieldIndexToPosInSchema;
    void cleanup();
    bool loadFromString(std::string& str);
    void sort (std::vector<int>& theResults, const	std::vector<SortCriterion>& crit, int critLocation);
    bool compare(int s1, int s2, const std::vector<SortCriterion>& crit, int critLocation, std::vector<int> critToSchema);
};

#endif /* defined(__Project_4__Database__) */
