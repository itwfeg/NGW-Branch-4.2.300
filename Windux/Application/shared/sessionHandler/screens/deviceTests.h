/**
 * @file deviceTests.h
 * Class that allows you to send tests to the label applier and wrapper.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Alex Zavala
 *
 * @addtogroup platformCsu
 * @{
 */
#ifndef DEVICETESTS_H
#define DEVICETESTS_H

#include "Session.h"
#include "Events.h"
#include "Devices.h"
#include <QPushButton>

class QPushButton;

class deviceTests
{
public:
    /**
     * Constructor
     *
     * @brief deviceTests
     */
    deviceTests();

    /**
      * Destructor
      *
      * @brief ~deviceTests
      */
    ~deviceTests();

    /**
     * A method to send a test to the wrapper
     *
     * @brief sendWrapperTest
     * @param session the session that is used to send the message
     * @param btn the button that was clicked on the ui
     * @param test the test number
     * @param subTest the subtest of the test
     * @param param the parameter for the test
     */
    void sendWrapperTest(Session* session, QPushButton* btn, int test, int subTest, int param);

    /**
     * A method to send a test to the label applier
     *
     * @brief sendLabelApplierTest
     * @param session the session that is used to send the message
     * @param btn the button that was clicked on the ui
     * @param test the test number
     * @param station the station this test is being sent to
     */
    void sendLabelApplierTest(Session* session, QPushButton* btn, int test, int station);

    /**
     * A method for completing a test
     *
     * @brief testComplete
     * @param test the test that was completed
     */
    void testComplete(int test);

    /**
     * A method for cancelling any test that is currently running
     *
     * @brief cancelCurrentTest
     */
    void cancelCurrentTest();

private:

    /**
     * @brief highlightButton
     */
    void highlightButton();

    /**
     * @brief unhighlightButton
     */
    void unhighlightButton();

    //A reference to the button that was clicked to run the test
    QPushButton* testButton;

    //The test that is currently being run
    int currentTest;
};

#endif // DEVICETESTS_H
