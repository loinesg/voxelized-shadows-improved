#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include <QElapsedTimer>
#include <QOpenGLFunctions_3_3_Core>

struct RenderPassStats
{
    // Basic render pass info
    std::string name;
    
    // The average pass time for the previous sample period
    double averageTime;
    
    // GPU time elapsed query
    GLuint query;
    
    // The current total pass time for the sample
    // currently being measured
    double sampleCurrentTime;
};

class RendererStats : protected QOpenGLFunctions_3_3_Core
{
private:
    const int FramesPerSample = 200;
    
public:
    RendererStats();
    ~RendererStats();
    
    int totalSamples() const { return totalSamples_; }
    
    // Gets the stats for each pass
    int passCount() const;
    const std::string& passName(int pass) const;
    double passAverageTime(int pass) const;
    
    // Adds another render pass to the stats system
    void addPass(const std::string &name);
    
    // Get the averaged results from the last samples
    double currentFrameRate() const { return avgFrameRate_; }
    double currentFrameTime() const { return avgFrameTime_; }
    
    // These methods are called at certain points in a frame by RendererWidget
    void frameStarted();
    void frameFinished();
    void passStarted(int passIndex);
    void passFinished();
    
private:
    
    // The timer used for measuring rendering times
    QElapsedTimer frameTimer_;
    
    // Information for each render pass
    std::vector<RenderPassStats> passStats_;
    
    // The pass that is currently in progress, or -1.
    int currentPass_;
    
    // The number of samples that have been displayed in the ui
    int totalSamples_;
    
    // The time that the current sample period started.
    // Sample periods last for FramesPerSample frames.
    qint64 samplePeriodStart_;
    
    // The number of frames that have so far counted towards
    // the current sample period.
    int sampleFrameCount_;
    
    // The last set of samples
    // These values are the ones currently displayed
    double avgFrameRate_;
    double avgFrameTime_;
};
