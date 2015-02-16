//
//  main.cpp
//  Project 4
//
//  Created by Jules Robins on 3/6/14.
//  Copyright (c) 2014 Jules Robins. All rights reserved.
//

#include <iostream>
#include <assert.h>
#include "MultiMap.h"
#include "Database.h"
#include "http.h"
#include <vector>

using namespace std;

int main(int argc, const char * argv[])
{

    Database db;
    Database::FieldDescriptor	fd1,	fd2,	fd3;
	
    fd1.name	=	"username";
    fd1.index	=	Database::it_indexed;	//	username	is	an	indexed	field
	
    fd2.name	=	"phonenum";
    fd2.index	=	Database::it_indexed;	//	phone	#	is	an	indexed	field
	
    fd3.name	=	"age";
    fd3.index	=	Database::it_none;	//	age	is	NOT	an	indexed	field
	
    std::vector<Database::FieldDescriptor>	schema;
    schema.push_back(fd1); 			schema.push_back(fd2); 			schema.push_back(fd3);
	
    db.specifySchema(schema);
    
    vector<string> row1 = {"billy", "704-323-8641", "34"};
    vector<string> row2 = {"person2", "704-323-8641", "39"};
    
    db.addRow(row1);
    db.addRow(row2);
    db.getRow(1, row2);
    db.addRow(row2);
    
    db.loadFromURL("http://cs.ucla.edu/classes/winter14/cs32/Projects/4/Data/fn_ln_age_kids_married_ssn_25k.csv");

    std::vector<Database::SearchCriterion>	searchCrit;
	
    Database::SearchCriterion	s1;
    s1.fieldName	=	"FirstName";
    s1.minValue	=	"";
    s1.maxValue	=	"MOLLY";
	
    Database::SearchCriterion	s2;
    s2.fieldName	=	"Age";
    s2.minValue	=	"018";		//	no	minimum	specified
    s2.maxValue	=	"099";
	
    searchCrit.push_back(s1);
    searchCrit.push_back(s2);
	
    //	We’ll	leave	our	sort	criteria	empty	for	now,	which	means
    //	the	results	may	be	returned	to	us	in	any	order
	
    std::vector<Database::SortCriterion>	sortCrit;
    
    Database::SortCriterion so1;
    so1.fieldName = "Married";
    so1.ordering = Database::ot_ascending;
    
    Database::SortCriterion so2;
    so2.fieldName = "FirstName";
    so2.ordering = Database::ot_descending;
    
    Database::SortCriterion so3;
    so3.fieldName = "SocialSecurityNum";
    so3.ordering = Database::ot_ascending;
    
    sortCrit.push_back(so1);
    sortCrit.push_back(so2);
    sortCrit.push_back(so3);
    
    std::vector<int>	results;
    int	numFound	=	db.search(searchCrit,	sortCrit,	results);
    if	(numFound	==	Database::ERROR_RESULT)
        cout	<<	"Error	querying	the	database!"	<<	endl;
    else
    {
        cout	<<	numFound	<<	"	rows	matched	the	criteria;	here	they	are:"	<<	endl;
        for	(int	i	=	0;	i	<	results.size();	i++)
        {
            //	print	the	row	number	out	where	we	had	a	match
            cout	<<	"Row	#"	<<	results[i]	<<	":	";
            
            //	get	and	print	the	field	values	out	from	that	row
            std::vector<std::string> rowData;
            if	(db.getRow(results[i],	rowData))
            {
                for	(int j = 0; j < rowData.size(); j++)
                {
                    std::cout << rowData[j] << " ";
                }
                std::cout	<<	endl;
            }
            else
                cout	<<	"Error	retrieving	row’s	data!"	<<	endl;
        }	
    }
    
    std::cout << "Passed\n";
    return 0;
}

