//
//  MultiMap.h
//  Project 4
//
//  Created by Jules Robins on 3/6/14.
//  Copyright (c) 2014 Jules Robins. All rights reserved.
//

#ifndef __Project_4__MultiMap__
#define __Project_4__MultiMap__

#include <string>

class	MultiMap
{
private:
    class Node
    {
    public:
        Node (std::string key, unsigned int val);
        std::string nKey() {return m_key;}
        unsigned int nVal() {return m_val;}
        Node* nPar() {return m_par;}
        Node* nLeft() {return m_left;}
        Node* nRight() {return m_right;}
        void setPar(Node* par) {m_par=par;}
        void setLeft(Node* left) {m_left=left;}
        void setRight(Node* right) {m_right=right;}
    private:
        std::string m_key;
        unsigned int m_val;
        Node* m_par;
        Node* m_left;
        Node* m_right;
    };
    Node* m_head;
    //	To	prevent	Multimaps	from	being	copied	or	assigned,	declare	these	members
    //	private	and	do	not	implement	them.
    MultiMap(const	MultiMap&	other);
    MultiMap&	operator=(const	MultiMap&	rhs);
    void eraseSub (Node* root); //used to allow recursion in the argument-less clear() function
public:
    class	Iterator
    {
    public:
        Iterator(); //default is invalid
        Iterator(Node* ptr); //iterator will correspond to passed node
        bool	valid()	const;
        std::string	getKey()	const;
        unsigned	int	getValue()	const;
        bool	next();
        bool	prev();
    private:
        Node* m_ptr;
    };
	
    MultiMap(); //defaults to empty
    ~MultiMap();
    void	clear();
    void	insert(std::string	key,	unsigned	int	value); //adds in a valid position
    Iterator	findEqual(std::string	key)	const;
    Iterator	findEqualOrSuccessor(std::string	key)	const;
    Iterator	findEqualOrPredecessor(std::string	key)	const;
};

#endif /* defined(__Project_4__MultiMap__) */
