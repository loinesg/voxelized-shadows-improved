#pragma once

#include <string>
#include <vector>

#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "RendererWidget.hpp"

struct TestViewpoint
{
    // The viewpoint name
    std::string name;
    
    // The location of the camera for the viewpoint
    Vector3 position;
    Quaternion rotation;
    
    // The resulting average frame time
    double frameTime;
    
    // The resulting average time for each pass
    double passTimes[4];
};

class PerformanceTest
{
public:
    PerformanceTest(RendererWidget* renderer);
    
    // Test info
    bool isRunning() const { return running_; }
    
    // Starts running the performance test
    void startTest();
    
    // Updates the performance test, if it is running
    void update();
    
private:
    RendererWidget* renderer_;
    
    bool running_;
    
    // The viewpoints that must be measured
    std::vector<TestViewpoint> viewpoints_;
    
    // The viewpoint index currently being measured.
    // -1 if not currently measuring
    int currentViewpoint_;
    
    // The sample count that the viewpoint measure is waiting
    // to be hit.
    // We wait for a few profiler samples to be made, as the
    // timings for a viewpoint may not settle down immediately.
    int currentViewpointSampleNum_;
    
    void startNextViewpoint();
    void viewpointFinished();
    void testFinished();
};
