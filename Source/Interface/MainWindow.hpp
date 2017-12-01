#pragma once

#include <QWidget>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QLabel>

#include "RendererWidget.hpp"

class MainWindow : public QWidget
{
public:
    MainWindow(bool fullScreen, const QGLFormat &format, int voxelResolution);

    // Renderer and side panel
    RendererWidget* rendererWidget() const { return rendererWidget_; }
    QWidget* sidePanelWidget() const { return sidePanelWidget_; }

    // Stats widgets
    QLabel* resolutionLabel() const { return resolutionLabel_; }
    QLabel* frameRateLabel() const { return frameRateLabel_; }
    QLabel** passRenderingTimeLabels() const { return passRenderingTimeLabels_; }
    QLabel* treeResolutionLabel() const { return treeResolutionLabel_; }
    QLabel* treeTilesLabel() const { return treeTilesLabel_; }
    QLabel* originalSizeLabel() const { return originalSizeLabel_; }
    QLabel* treeSizeLabel() const { return treeSizeLabel_; }
    
    // Render setting widget lists
    QObjectList shaderFeatureToggles() { return featureToggles_->children(); }
    QObjectList shadowMethodRadios() { return shadowMethodRadios_->children(); }
    QObjectList overlayRadios() { return overlayRadios_->children(); }
    QObjectList shadowResolutionRadios() { return shadowResolutionRadios_->children(); }
    QObjectList shadowCascadesRadios() { return shadowCascadesRadios_->children(); }
    QObjectList voxelPCFFilterSizeRadios() { return voxelPCFFilterSizeRadios_->children(); }
    
private:
    
    // Main renderer widget
    RendererWidget* rendererWidget_;
    
    // Side panel widget
    QWidget* sidePanelWidget_;
    
    // Stats labels
    QGroupBox* statsGroupBox_;
    QLabel* resolutionLabel_;
    QLabel* frameRateLabel_;
    QLabel** passRenderingTimeLabels_;
    QLabel* treeResolutionLabel_;
    QLabel* treeTilesLabel_;
    QLabel* originalSizeLabel_;
    QLabel* treeSizeLabel_;
    
    // Render setting widget groups
    QGroupBox* featureToggles_;
    QGroupBox* shadowMethodRadios_;
    QGroupBox* overlayRadios_;
    QGroupBox* shadowResolutionRadios_;
    QGroupBox* shadowCascadesRadios_;
    QGroupBox* voxelPCFFilterSizeRadios_;
    
    QLabel* createStatsLabel();
    QCheckBox* createFeatureToggle(ShaderFeature feature, const char* label);
    QRadioButton* createShadowMethodRadio(ShadowMaskMethod method, const char* label);
    QRadioButton* createOverlayRadio(int index, const char* label);
    QRadioButton* createShadowResolutionRadio(int resolution);
    QRadioButton* createShadowCascadesRadio(int cascades);
    QRadioButton* createVoxelPCFFilterSizeRadio(int kernelSize);
};
