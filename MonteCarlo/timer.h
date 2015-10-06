//
//  timer.h
//  Parallel_HW5
//
//  Created by James Williams on 9/23/15.
//  Copyright © 2015 James Williams. All rights reserved.
//

#ifndef timer_h
#define timer_h


class timer
{
private:
    unsigned long begTime;
    
public:
    void start()
    {
        begTime = clock();
    }
    
    unsigned long elapsedTime()
    {
        return ((unsigned long) clock() - begTime);
    }
};

#endif /* timer_h */
