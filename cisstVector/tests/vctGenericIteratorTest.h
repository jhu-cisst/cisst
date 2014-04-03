/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Ofri Sadowsky
  Created on: 2003-09-30
  
  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _vctGenericIteratorTest_h
#define _vctGenericIteratorTest_h

#include <cppunit/extensions/HelperMacros.h>

/*!  class GenericIteratorTest is made to test the abstract operations on an
  iterator, as defined in the STL documentation.  See for example
  http://www.sgi.com/tech/stl/ or http://www.roguewave.com/support/docs/sourcepro/stdlibref/

  This class defined functions to test invariants of iterator operations, for example,
  the equivalence of different addition operations (++, +=, +).
*/
template<class _container>
class GenericIteratorTest
{
 public:
    typedef _container ContainerType;
    typedef typename ContainerType::const_iterator ConstIteratorType;
    typedef typename ContainerType::iterator IteratorType;
    typedef typename ContainerType::difference_type difference_type;
    
    /*! Assert (it1 == it2) && (it2 == it1).  This function is useful in many
      occasions where we want to verify that two iterators are identical.
    */
    static void TestSymmetricEquality(const ConstIteratorType & it1,
                                      const ConstIteratorType & it2) 
    {
        CPPUNIT_ASSERT( (it1 == it2) && (it2 == it1) );
    }
    

    /*! Increment the iterator value, decrement, the result, and assert that
      it returns to the initial state. */
    static void TestIncrementDecrement(ConstIteratorType & iterator) {
        const ConstIteratorType oldValue(iterator);
        ++iterator;
        --iterator;
        TestSymmetricEquality(oldValue, iterator);
    }
    

    /*! Decrement the iterator value, increment, the result, and assert that
      it returns to the initial state. */
    static void TestDecrementIncrement(ConstIteratorType & iterator) {
        const ConstIteratorType oldValue(iterator);
        --iterator;
        ++iterator;
        TestSymmetricEquality(oldValue, iterator);
    }

    /*! Test the properties of pre-increment (++x) and post-increment (x++) operations:
      While evaluationg an expression where initally (y == z) is true, (++y != z++) should 
      evaluate to true.  After the evaluation, we should again have (y == z).  Same for
      -- operators.
    */
    static void TestPreAndPostIncrementAndDecrement(ConstIteratorType & iterator)
    {
        ConstIteratorType iterator1 = iterator;
        ConstIteratorType iterator2 = iterator;
        CPPUNIT_ASSERT( ++iterator1 != iterator2++ );
        TestSymmetricEquality(iterator1, iterator2);
        CPPUNIT_ASSERT( ++iterator1 > iterator2++ );
        CPPUNIT_ASSERT( --iterator1 < iterator2-- );
        CPPUNIT_ASSERT( --iterator1 != iterator2-- );
        TestSymmetricEquality(iterator1, iterator2);
    }

    /*! Run an iterator from the beginning to the end of the container and count how
      many times it is incremented.  Assert that this count is equal to the reported size
      of the container. */
    static void TestContainerSizeForIncrement(const ContainerType & container) {
        ConstIteratorType iterator = container.begin();
        ConstIteratorType endIterator = container.end();
        typename ContainerType::size_type counter = 0;
        while (iterator != endIterator) {
            ++iterator;
            ++counter;
        }
        CPPUNIT_ASSERT(counter == container.size());
    }
    

    /*! Run an iterator from the end to the beginning of the container and count how
      many times it is decremented.  Assert that this count is equal to the reported size
      of the container. */
    static void TestContainerSizeForDecrement(const ContainerType & container) {
        ConstIteratorType iterator = container.end();
        ConstIteratorType endIterator = container.begin();
        typename ContainerType::size_type counter = 0;
        while (iterator != endIterator) {
            --iterator;
            ++counter;
        }
        CPPUNIT_ASSERT(counter == container.size());
    }
    

    /*! Run a reverse iterator from the beginning to the end of the reversed container and count 
      how many times it is incremented.  Assert that this count is equal to the reported size
      of the container. */
    static void TestContainerSizeForReverseIncrement(const ContainerType & container) {
        typename ContainerType::const_reverse_iterator iterator = container.rbegin();
        typename ContainerType::const_reverse_iterator endIterator = container.rend();
        typename ContainerType::size_type counter = 0;
        while (iterator != endIterator) {
            ++iterator;
            ++counter;
        }
        CPPUNIT_ASSERT(counter == container.size());
    }
    

    /*! Run a reverse iterator from the end to the beginning of the reversed container and count 
      how many times it is decremented.  Assert that this count is equal to the reported size
      of the container. */
    static void TestContainerSizeForReverseDecrement(const ContainerType & container) {
        typename ContainerType::const_reverse_iterator iterator = container.rend();
        typename ContainerType::const_reverse_iterator endIterator = container.rbegin();
        typename ContainerType::size_type counter = 0;
        while (iterator != endIterator) {
            --iterator;
            ++counter;
        }
        CPPUNIT_ASSERT(counter == container.size());
    }


    /*! Test the following for iterator i and difference d (with some notation abuse!):
      1)  i+d == d+i
      2)  (i += d) == i+d
      3) i+d-d == i
      4) (i+d)-i == d
      5) (i -= d) == (i - d)
    */
    static void TestAdditionAndSubtractionSymmetry(const ConstIteratorType & iterator,
                                                   difference_type difference) 
    {
        ConstIteratorType it1 = iterator + difference;
        ConstIteratorType it2 = difference + iterator;
        ConstIteratorType it3 = iterator;
        it3 += difference;
        TestSymmetricEquality(it1, it2);
        TestSymmetricEquality(it1, it3);
        CPPUNIT_ASSERT( &(iterator[difference]) == &(*it1) );
        
        it3 -= difference;
        TestSymmetricEquality( (it1 - difference), iterator );
        difference_type iteratorDiff = it1 - iterator;
        CPPUNIT_ASSERT( iteratorDiff == difference );
        TestSymmetricEquality(it3, iterator);
    }


    /*! Test for the equivalence of the + operation and a sequence of ++ operations
      on an iterator.  Do the same with - and a sequence of --.  The difference argument
      must be non-negative.
    */
    static void TestAdditionAndIncrementEquivalence(const ConstIteratorType & iterator,
                                                    difference_type difference) 
    {
        ConstIteratorType it1 = iterator;
        ConstIteratorType it2 = iterator;
        int counter = 0;
        for (; counter < difference; ++counter) {
            ++it1;
            --it2;
        }
        
        CPPUNIT_ASSERT(it1 == (iterator + difference));
        CPPUNIT_ASSERT( (it2 + difference) == iterator );
    }

    /*! Assert that by adding a positive difference to an iterator we get one which is ``greater than''
      the initial iterator, and that by adding a negative difference we get a ``lesser'' one.  The 
      difference argument may be of any sign, and we rectify the sign in the function.
    */
    static void TestOrdering(const ConstIteratorType & iterator,
                             difference_type difference) 
    {
        ConstIteratorType it1 = (difference > 0) ? (iterator + difference) : (iterator - difference);
        ConstIteratorType it2 = (difference > 0) ? (iterator - difference) : (iterator + difference);

        if (difference != 0) {
            CPPUNIT_ASSERT( iterator < it1 );
            CPPUNIT_ASSERT( it2 < iterator );
            CPPUNIT_ASSERT( it2 < it1 );
            CPPUNIT_ASSERT( it2 != it1 );
        }
    }
};


#endif   // _vctGenericIteratorTest_h

