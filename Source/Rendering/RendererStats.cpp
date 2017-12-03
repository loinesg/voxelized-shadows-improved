#include "RendererStats.hpp"

RendererStats::RendererStats()
    : frameTimer_(),
    passStats_(),
    currentPass_(-1),
    totalSamples_(0),
    samplePeriodStart_(0),
    sampleFrameCount_(0),
    avgFrameRate_(-1),
    avgFrameTime_(-1)
{
    // Start the frame time timers
    frameTimer_.start();
    
    // Enable support for opengl 3.3 features
    initializeOpenGLFunctions();
}

RendererStats::~RendererStats()
{
    // Delete all created gl queries
    for(auto pass = passStats_.begin(); pass != passStats_.end(); ++pass)
    {
        glDeleteQueries(1, &pass->query);
    }
}

int RendererStats::passCount() const
{
    return (int)passStats_.size();
}

const std::string& RendererStats::passName(int pass) const
{
    return passStats_[pass].name;
}

double RendererStats::passAverageTime(int pass) const
{
    return passStats_[pass].averageTime;
}

void RendererStats::addPass(const std::string &name)
{
    // Create the new pass
    RenderPassStats pass;
    pass.name = name;
    
    // Create a query object to measure gpu time
    glGenQueries(1, &pass.query);
    
    // Add the pass to the passes list
    passStats_.push_back(pass);
}

void RendererStats::frameStarted()
{
    // If a frame has been recorded, read back the pass start and end times
    if(sampleFrameCount_ > 0)
    {
        for(auto pass = passStats_.begin(); pass != passStats_.end(); ++pass)
        {
            // Get the start and end times of the pass
            GLint64 elapsed;
            glGetQueryObjecti64v(pass->query, GL_QUERY_RESULT, &elapsed);
            
            // Convert the elapsed time from nanoseconds to milliseconds
            double elapsedMS = elapsed / (double)1000000.0;
            
            // Add to the pass time
            pass->sampleCurrentTime += elapsedMS;
        }
    }
    
    // If a sample period has just finished, record the results
    if(sampleFrameCount_ == FramesPerSample)
    {
        // Measure the sample time difference
        qint64 samplePeriodEnd = frameTimer_.elapsed();
        qint64 samplePeriodDuration = (samplePeriodEnd - samplePeriodStart_);
        
        // Store the sample frame rate and frame time
        avgFrameTime_ = samplePeriodDuration / (double)FramesPerSample;
        avgFrameRate_ = 1000.0 / (double)avgFrameTime_;
        
        // Compute the average time for each pass
        for(auto pass = passStats_.begin(); pass != passStats_.end(); ++pass)
        {
            pass->averageTime = pass->sampleCurrentTime / (double)FramesPerSample;
            pass->sampleCurrentTime = 0.0;
        }
        
        // Reset the samples count
        sampleFrameCount_ = 0;
        totalSamples_ ++;
    }
    
    // If a new sample period has just started, record the start time
    if(sampleFrameCount_ == 0)
    {
        samplePeriodStart_ = frameTimer_.elapsed();
    }
    
    // Reset the pass count
    currentPass_ = -1;
}

void RendererStats::frameFinished()
{
    // End any pass that is in progress
    passFinished();
    
    // Increment the sample count
    sampleFrameCount_ ++;
}

void RendererStats::passStarted(int passIndex)
{
    // End any pass that is in progress
    passFinished();
    
    // Record the pass that has now started
    currentPass_ = passIndex;
    
    // Kick off the gpu timer
    glBeginQuery(GL_TIME_ELAPSED, passStats_[currentPass_].query);
}

void RendererStats::passFinished()
{
    // Check that a pass was actually in progress
    if(currentPass_ == -1)
    {
        return;
    }
    
    // Stop the gpu timer
    glEndQuery(GL_TIME_ELAPSED);
}
