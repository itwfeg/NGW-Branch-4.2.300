/* @file deviceTests.cpp
 * deviceTests
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala

   @Brief This file contains all the functions necessary for testing the wrapper and label applier
 */

#include "deviceTests.h"

#include <QPushButton>

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

deviceTests::deviceTests()
{
    testButton = NULL;
    currentTest = -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
deviceTests::~deviceTests()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceTests::sendWrapperTest(Session* session, QPushButton* btn, int test, int subTest, int param)
{
    if(currentTest == -1)//Not running a test
    {
        QString msg = QString::number(test);
        if(subTest != -1)
        {
            msg += "," + QString::number(subTest);
            if(param != -1)
            {
                msg += "," + QString::number(param);
            }
        }

        //Send the test and prevent new tests from coming in
        session->send(Wrapper, WrEvolTest, msg);
        testButton = btn;
        currentTest = test;
        highlightButton();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceTests::sendLabelApplierTest(Session* session, QPushButton* btn, int test, int station)
{
    if(currentTest == -1)//Not running a test
    {
        QString msg = QString::number(station) + "," + QString::number(test);

        //Send the test and prevent new tests from coming in
        session->send(LabelApplier, LaTest, msg);
        currentTest = test;
        testButton = btn;
        highlightButton();
    }
    else if(test == LaTestRunInOff && currentTest == LaTestRunInOn)//Special case for CLA mfg. run in
    {
        QString msg = QString::number(station) + "," + QString::number(test);
        session->send(LabelApplier, LaTest, msg);
        currentTest = -1;
        unhighlightButton();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceTests::highlightButton()
{
    if(testButton != NULL)
    {
        testButton->setStyleSheet("background-color: green");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceTests::unhighlightButton()
{
    if(testButton != NULL)
    {
        testButton->setStyleSheet("");
        testButton = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceTests::cancelCurrentTest()
{
    testButton = NULL;
    currentTest = -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceTests::testComplete(int test)
{
    if(currentTest == test)
    {
        unhighlightButton();
        currentTest = -1;
    }
}
