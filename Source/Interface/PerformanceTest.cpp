#include "PerformanceTest.hpp"

#include <fstream>

#include "Platform.hpp"

PerformanceTest::PerformanceTest(RendererWidget* renderer)
    : renderer_(renderer),
    running_(false),
    viewpoints_(),
    currentViewpoint_(-1),
    currentViewpointSampleNum_(-1)
{
    // Get the location of the viewpoint locations file
    const std::string fileName = "scene-test-positions.txt";
    const std::string filePath = SCENES_DIRECTORY + fileName;
    
    // Load in the viewpoint locations
    std::ifstream file(filePath);
    while(!file.eof())
    {
        TestViewpoint viewpoint;
        file >> viewpoint.name;
        file >> viewpoint.position;
        file >> viewpoint.rotation;
        viewpoints_.push_back(viewpoint);
    }
}

void PerformanceTest::startTest()
{
    running_ = true;
}

void PerformanceTest::update()
{
    // Do nothing if the test is not running
    if(running_ == false)
    {
        return;
    }
    
    // Do nothing if the voxel tree is not built
    if(renderer_->voxelTree()->totalTiles() != renderer_->voxelTree()->completedTiles())
    {
        return;
    }
    
    // If we have not yet started a viewpoint, start one
    if(currentViewpoint_ == -1)
    {
        startNextViewpoint();
    }
    
    // If we have been waiting on a profiler sample and it
    // is ready, record the result.
    const RendererStats* stats = renderer_->stats();
    if(currentViewpointSampleNum_ == stats->totalSamples())
    {
        viewpointFinished();
    }
}

void PerformanceTest::startNextViewpoint()
{
    // Update the currentViewport index
    currentViewpoint_ ++;
    
    // Check if there are no more viewpoints to do
    if(currentViewpoint_ == (int)viewpoints_.size())
    {
        testFinished();
        return;
    }
    
    // Move the camera to the correct place
    Camera* camera = renderer_->scene()->mainCamera();
    camera->setPosition(viewpoints_[currentViewpoint_].position);
    camera->setRotation(viewpoints_[currentViewpoint_].rotation);
    
    // Wait for several profiler samples before recording the performance
    // This gives the frame rate a chance to settle down
    currentViewpointSampleNum_ = renderer_->stats()->totalSamples() + 2;
}

void PerformanceTest::viewpointFinished()
{
    const RendererStats* stats = renderer_->stats();
    
    // Record the times for the viewport
    viewpoints_[currentViewpoint_].frameTime = stats->currentFrameTime();
    for(int p = 0; p < 4; ++p)
    {
        viewpoints_[currentViewpoint_].passTimes[p] = stats->passAverageTime(p);
    }
    
    // Move on to the next viewport
    startNextViewpoint();
}

void PerformanceTest::testFinished()
{
    // Get the location of the output file
    const std::string fileName = "performance-results.csv";
    
    // Write the csv row headers first
    ofstream file(fileName);
    file << "Viewpoint,Frame Time";
    for(int pass = 0; pass < 4; ++pass)
    {
        file << "," << renderer_->stats()->passName(pass);
    }
    file << endl;
    
    // Now write the results
    for(auto viewpoint = viewpoints_.begin(); viewpoint != viewpoints_.end(); viewpoint++)
    {
        file << viewpoint->name << ","
            << viewpoint->frameTime << ","
            << viewpoint->passTimes[0] << ","
            << viewpoint->passTimes[1] << ","
            << viewpoint->passTimes[2] << ","
            << viewpoint->passTimes[3] << endl;
    }
    
    // Store the averages
    double averageFrameTime = 0.0;
    double averagePassTimes[4];
    for(int i = 0; i < 4; ++i)
    {
        averagePassTimes[i] = 0.0;
    }
    
    // Include each viewpoint in the average times
    for(auto viewpoint = viewpoints_.begin(); viewpoint != viewpoints_.end(); viewpoint++)
    {
        averageFrameTime += viewpoint->frameTime;
        for(int pass = 0; pass < 4; ++pass)
        {
            averagePassTimes[pass] += viewpoint->passTimes[pass];
        }
    }
    
    // Finally output the average results
    file << "Average" << ","
        << averageFrameTime / (double)viewpoints_.size() << ","
        << averagePassTimes[0] / (double)viewpoints_.size() << ","
        << averagePassTimes[1] / (double)viewpoints_.size() << ","
        << averagePassTimes[2] / (double)viewpoints_.size() << ","
        << averagePassTimes[3] / (double)viewpoints_.size() << endl;
    
    // Done.
    file.close();
    running_ = false;
}
