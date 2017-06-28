
#ifndef MM_TRAINING_RUNNABLE_H
#define MM_TRAINING_RUNNABLE_H

/**
 * Runnable
 * @author ianhua
 */

namespace mmtraining {

/**
 * Runnable abstract class
 */
class Runnable {
public:
    /**
     * Destuctor
     */
    virtual ~Runnable() {}

    /**
     * Run abstract funciton 
     */
    virtual int Run() = 0;
};

}  // mmtraining

#endif
