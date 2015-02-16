//
//  MultiMap.cpp
//  Project 4
//
//  Created by Jules Robins on 3/6/14.
//  Copyright (c) 2014 Jules Robins. All rights reserved.
//

#include "MultiMap.h"

MultiMap::MultiMap()
{m_head=NULL;}

MultiMap::~MultiMap() {clear();}

void	MultiMap::clear()
{eraseSub(m_head);}

void MultiMap::eraseSub (Node* root)
{
    if (root!=NULL) //postorder traversal to ensure no memory leak due to parent deletion
    {
        eraseSub(root->nLeft());
        eraseSub(root->nRight());
    }
    delete root;
}

void	MultiMap::insert(std::string	key,	unsigned	int	value)
{
    Node* fresh = new Node(key, value);
    if (m_head==NULL) //empty map
    {m_head=fresh; return;}
    Node* mover=m_head;
    while (true)
    {
        if (mover->nKey()<key && mover->nRight()!=NULL) {mover=mover->nRight();} //bigger
        else if (mover->nKey()>key && mover->nLeft()!=NULL) {mover=mover->nLeft();} //smaller
        else {break;}
    }
    fresh->setPar(mover);
    if (mover->nKey()<key) //bigger
    {mover->setRight(fresh);}
    else
    {
        fresh->setLeft(mover->nLeft());
        if (mover->nLeft()!=NULL) //equal and had a left child already
        {mover->nLeft()->setPar(fresh);}
        mover->setLeft(fresh);
    }
}

MultiMap::Iterator	MultiMap::findEqual(std::string	key)	const
{
    Node* trav = m_head;
    Iterator f;
    while (trav!=NULL)
    {
        if (trav->nKey()==key)
        {
            Iterator i(trav);
            return i;
        }
        else if (trav->nKey() < key && trav->nRight()!=NULL)
        {
            trav=trav->nRight();
        }
        else if (trav->nKey() > key && trav->nLeft()!=NULL)
        {
            trav=trav->nLeft();
        }
        else
        {
            return f;
        }
    }
    return f;
}

MultiMap::Iterator	MultiMap::findEqualOrSuccessor(std::string key)	const
{
    Node* trav = m_head;
    Iterator f;
    while (trav!=NULL)
    {
        if (trav->nKey() < key && trav->nRight()!=NULL)
        {
            trav=trav->nRight();
        }
        else if (trav->nKey() >= key && trav->nLeft()!=NULL)
        {
            if (trav->nKey()==key && trav->nLeft()->nKey()!=key) //first equal
            {
                Iterator i(trav);
                return i;
            }
            trav=trav->nLeft();
        }
        else if (trav->nKey() >= key && trav->nLeft()==NULL) //smallest above
        {
            Iterator ii(trav);
            return ii;
        }
        else
        {
            return f;
        }
    }
    return f;
}

MultiMap::Iterator	MultiMap::findEqualOrPredecessor(std::string key)	const
{
    Node* trav = m_head;
    Iterator f;
    while (trav!=NULL)
    {
        if (trav->nKey() > key && trav->nLeft()!=NULL)
        {
            trav=trav->nLeft();
        }
        else if (trav->nKey() <= key && trav->nRight()!=NULL)
        {
            if (trav->nKey()==key && trav->nRight()->nKey()!=key) //last equal
            {
                Iterator i(trav);
                return i;
            }
            trav=trav->nRight();
        }
        else if (trav->nKey() <= key && trav->nRight()==NULL) //biggest below
        {
            Iterator ii(trav);
            return ii;
        }
        else
        {
            return f;
        }
    }
    return f;
}



MultiMap::Iterator::Iterator()
{
    m_ptr=NULL;
}

MultiMap::Iterator::Iterator(Node* ptr)
{
    m_ptr=ptr;
}

bool	MultiMap::Iterator::valid()	const
{return m_ptr!=NULL;}

std::string	MultiMap::Iterator::getKey()	const
{return m_ptr->nKey();}

unsigned	int	MultiMap::Iterator::getValue()	const
{return m_ptr->nVal();}

bool	MultiMap::Iterator::next()
{
    if (!valid())
    {return false;}
    Node* nextVal = m_ptr;
    if (nextVal->nRight()!=NULL) //check for larger child
    {
        nextVal=nextVal->nRight();
        while (nextVal->nLeft()!=NULL)
        {nextVal=nextVal->nLeft();}
        m_ptr=nextVal;
        return true;
    }
    while (nextVal->nPar()!=NULL) //find ancestor which this is in the left branch of
    {
        if (nextVal->nPar()->nLeft()==nextVal)
        {
            m_ptr=nextVal->nPar();
            return true;
        }
        nextVal=nextVal->nPar();
    }
    m_ptr=NULL; //otherwise it's the last one
    return false;
}

bool	MultiMap::Iterator::prev()
{
    if (!valid())
    {return false;}
    Node* nextVal = m_ptr;
    if (nextVal->nLeft()!=NULL) //find smaller child
    {
        nextVal=nextVal->nLeft();
        while (nextVal->nRight()!=NULL)
        {nextVal=nextVal->nRight();}
        m_ptr=nextVal;
        return true;
    }
    while (nextVal->nPar()!=NULL) //find ancestor which this is in the right branch of
    {
        if (nextVal->nPar()->nRight()==nextVal)
        {
            m_ptr=nextVal->nPar();
            return true;
        }
        nextVal=nextVal->nPar();
    }
    m_ptr=NULL; //otherwise it's the first one
    return false;
}



MultiMap::Node::Node(std::string key, unsigned int val)
{
    m_key=key;
    m_val=val;
    m_par=NULL;
    m_left=NULL;
    m_right=NULL;
}