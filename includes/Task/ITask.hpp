//
// Created by jusra on 2-1-2026.
//

#ifndef NEXUSCORE_ITASK_HPP
#define NEXUSCORE_ITASK_HPP

class ITask {
public:
    virtual ~ITask() = default;
    virtual void execute() = 0;
};


#endif //NEXUSCORE_ITASK_HPP