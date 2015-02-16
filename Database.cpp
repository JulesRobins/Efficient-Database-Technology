//
//  Database.cpp
//  Project 4
//
//  Created by Jules Robins on 3/9/14.
//  Copyright (c) 2014 Jules Robins. All rights reserved.
//

#include "MultiMap.h"
#include "Database.h"
#include "Tokenizer.h"
#include "http.h"
#include <unordered_set>
#include <iostream>
#include <fstream>

using namespace std;

Database::Database(){}

Database::~Database()
{
    cleanup();
}

bool	Database::specifySchema(const	vector<FieldDescriptor>&	schema)
{
    cleanup();
    m_rows.clear();
    m_fieldIndex.clear();
    m_schema.clear();
    m_fieldIndexToPosInSchema.clear();
    for (int i=0; i<schema.size(); i++)
    {
        if (schema[i].index==it_indexed) //create trees for indexed fields
        {
            MultiMap* sc = new MultiMap;
            m_fieldIndex.push_back(sc);
            m_fieldIndexToPosInSchema.push_back(i);
        }
    }
    if (m_fieldIndex.size()==0) //must have at least one index field
    {return false;}
    m_schema=schema;
    return true;
}

bool	Database::addRow(const	vector<string>&	rowOfData)
{
    if (rowOfData.size()!=m_schema.size()) //check for correct number of fields
    {return false;}
    m_rows.push_back(rowOfData);
    int counter=0;
    for (int i=0; i<rowOfData.size() && counter<m_fieldIndex.size(); i++) //go through each field until done with indexed ones
    {
        if (m_schema[i].index==it_indexed)
        {
            m_fieldIndex[counter]->insert(rowOfData[i], int(m_rows.size())-1); //add appropriate values to trees
            counter++;
        }
    }
    return true;
}

bool Database::loadFromString(string& str)
{
    string holdLine;
    string holdBit;
    string name;
    IndexType index;
    FieldDescriptor f;
    vector<FieldDescriptor> schema;
    vector<string> holdRow;
    if (str.length()<2)
    {
        vector<FieldDescriptor> f;
        specifySchema(f);
        return false;
    }
    Tokenizer lines (str, "\n");
    if (lines.getNextToken(holdLine)) //first line should specify schema
    {
        Tokenizer schemes (holdLine, ",");
        while (schemes.getNextToken(holdBit))
        {
            name="";
            for (int i=0; i<holdBit.length()-1; i++) //all but the last character must be part of the name
            {
                name+=holdBit[i];
            }
            if (holdBit[holdBit.length()-1]=='*') //which could instead indicate that it's indexed
            {
                index = it_indexed;
            }
            else
            {
                name+=holdBit[holdBit.length()-1];
                index = it_none;
            }
            f.name=name;
            f.index=index;
            schema.push_back(f);
        }
        specifySchema(schema);
    }
    else
    {return false;}
    while (lines.getNextToken(holdLine)) //the rest of the lines are rows
    {
        Tokenizer members (holdLine, ",");
        holdRow.clear();
        while (members.getNextToken(holdBit))
        {
            holdRow.push_back(holdBit);
        }
        if (holdRow.size()!=schema.size())
        {return false;}
        if(!addRow(holdRow))
        {return false;}
    }
    return true;
}

bool	Database::loadFromURL(string	url)
{
    string page;
    if (HTTP().get(url, page)) //puts content of the url into page
    {return loadFromString(page);}
    vector<FieldDescriptor> f;
    specifySchema(f);
    return false;
}

bool	Database::loadFromFile(string	filename)
{
    string page="";
    char c;
    ifstream file(filename.c_str());
    if (file)
    {
        while (file) //puts each character into page
        {
            file.get(c);
            page+=c;
        }
        return loadFromString(page);
    }
    vector<FieldDescriptor> f;
    specifySchema(f);
    return false;
}

int	Database::getNumRows()	const
{return int(m_rows.size());}

bool	Database::getRow(int	rowNum,	vector<string>&	row)	const
{
    if (m_rows.size()<=rowNum)
    {return false;}
    row=m_rows[rowNum];
    return true;
}

int	Database::search(const	vector<SearchCriterion>&	searchCriteria,
           const	vector<SortCriterion>&	sortCriteria,
           vector<int>&	results)
{
    results.clear();
    vector<int> theResults;
    if (searchCriteria.size()==0)
    {
        return ERROR_RESULT;
    }
    int counter=0;
    vector<unordered_set<int>> allResults;
    unordered_set<int> aResult;
    MultiMap::Iterator it;
    bool isValidCrit = false;
    for (int a=0; a<searchCriteria.size(); a++) //ensure all search criteria are valid
    {
        for (int b=0; b<m_fieldIndex.size(); b++)
        {
            if (m_schema[m_fieldIndexToPosInSchema[b]].name==searchCriteria[a].fieldName && m_schema[m_fieldIndexToPosInSchema[b]].index==it_indexed)
            {
                isValidCrit=true;
                break;
            }
        }
        if (!isValidCrit)
        {
            return ERROR_RESULT;
        }
        isValidCrit=false;
    }
    for (int k=0; k<m_fieldIndex.size() && counter!=searchCriteria.size(); k++) //find fields with matching names to check
    {
        for (int i=0; i<searchCriteria.size(); i++)
        {
            if (m_schema[m_fieldIndexToPosInSchema[k]].name==searchCriteria[i].fieldName)
            {
                counter++; //track how many you've gone through
                aResult.clear();
                if (searchCriteria[i].minValue!="") //has a min
                {
                    it = m_fieldIndex[k]->findEqualOrSuccessor(searchCriteria[i].minValue);
                    if (searchCriteria[i].maxValue!="") //has a max
                    {
                        while (it.valid() && it.getKey()<=searchCriteria[i].maxValue)
                        {
                            aResult.insert(it.getValue());
                            it.next();
                        }
                    }
                    else //no max
                    {
                        while (it.valid())
                        {
                            aResult.insert(it.getValue());
                            it.next();
                        }
                    }
                }
                else if (searchCriteria[i].maxValue!="") //has a max
                {
                    it = m_fieldIndex[k]->findEqualOrPredecessor(searchCriteria[i].maxValue);
                    if (searchCriteria[i].minValue!="") //has a min
                    {
                        while (it.valid() && it.getKey()>=searchCriteria[i].minValue)
                        {
                            aResult.insert(it.getValue());
                            it.prev();
                        }
                    }
                    else //no min
                    {
                        while (it.valid())
                        {
                            aResult.insert(it.getValue());
                            it.prev();
                        }
                    }
                }
                else //no min or max
                {
                    return ERROR_RESULT;
                }
                allResults.push_back(aResult);
                goto nextK; //done with that indexed field
            }
        }
    nextK:
        ;
    }
    bool checked=false;
    for (unordered_set<int>::iterator itr = allResults[0].begin(); itr != allResults[0].end() && allResults.size()>1;) //each match for first criterion
    {
        for (int i=1; i<allResults.size(); i++) //must appear in all search criteria
        {
            if (allResults[i].count(*itr)==0 && !checked)
            {
                unordered_set<int>::iterator temp = itr;
                temp++;
                allResults[0].erase(itr); //otherwise erase it
                itr=temp;
                checked=true;
                goto next; //and look at the next
            }
        }
        itr++;
        checked=false;
    next:
        ;
    }
    for (unordered_set<int>::iterator itr = allResults[0].begin(); itr != allResults[0].end(); itr++)
    {
        theResults.push_back(*itr);
    }
    sort(theResults, sortCriteria, 0); //sort
    results=theResults; //then set
    return int(allResults[0].size()); //and return the number of matching rows
}

void Database::sort (vector<int>& theResults, const vector<SortCriterion>& crit, int critLocation)
{
    vector<int> critToSchema;
    for (int m=0; m<crit.size(); m++)
    {
        for (int n=0; n<m_schema.size(); n++)
        {
            if (crit[m].fieldName==m_schema[n].name) //maps citeria index (index) to schema index (value)
            {
                critToSchema.push_back(n);
            }
        }
    }
    int maxChildPos;
    int parPos;
    int resHold;
    for (int i=int(theResults.size()-1); i>=0; i--)
    {
        int trek=i;
        while ((trek-1)/2 >= 0) //go til you hit the top
        {
            parPos=(trek-1)/2;
            if (compare(theResults[trek], theResults[parPos], crit, critLocation, critToSchema)) //if it has smaller parent
            {
                resHold = theResults[trek];
                theResults[trek]=theResults[parPos];
                theResults[parPos] = resHold;
                trek=parPos;
            }
            else
            {
                break; //stop if smaller than parents
            }
        }
    }
    //swap biggest element with end and leave it out of the newly formed heap
    int size=int(theResults.size());
    for (int l=0; size>0;)
    {
        resHold = theResults[l]; //swap with end
        theResults[l]=theResults[size-1];
        theResults[size-1] = resHold;
        size--;
        int trav=l;
        while (2*trav+1 < size) //check for children for new top
        {
            maxChildPos=2*trav+1;
            if (2*trav+2 < size)
            {
                if (compare(theResults[2*trav+2], theResults[2*trav+1], crit, critLocation, critToSchema))
                {
                    maxChildPos++;
                }
            }
            if (compare(theResults[maxChildPos], theResults[trav], crit, critLocation, critToSchema)) //if it has bigger children
            {
                resHold = theResults[trav];
                theResults[trav]=theResults[maxChildPos];
                theResults[maxChildPos] = resHold;
                trav=maxChildPos;
            }
            else
            {
                break;
            }
        }
    }
}

bool Database::compare(int s1, int s2, const vector<SortCriterion>& crit, int critLocation, vector<int> critToSchema)
{
    
    if (m_rows[s1][critToSchema[critLocation]]==m_rows[s2][critToSchema[critLocation]] && critLocation+1 < crit.size())
    {
        return compare(s1, s2, crit, critLocation+1, critToSchema); //check next criterion down in ties
    }
    if (crit[critLocation].ordering==ot_ascending) //otherwise compare with current criterion
    {
        return m_rows[s1][critToSchema[critLocation]]>m_rows[s2][critToSchema[critLocation]];
    }
    else
    {
        return m_rows[s1][critToSchema[critLocation]]<m_rows[s2][critToSchema[critLocation]];
    }
}

void Database::cleanup()
{
    for (int i=0; i<m_fieldIndex.size(); i++)
    {
        delete m_fieldIndex[i];
    }
}