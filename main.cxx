/*=Plus=header=begin======================================================
Program: Plus
Console app to connect to a plus device
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkPlusOpticalMarkerTracker.h"
#include "vtkPlusDevice.h"
#include "vtkPlusTransformRepository.h"
#include "vtkPlusDataCollector.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLDataParser.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyle.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkActor.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkCommand.h"
#include "vtkRendererCollection.h"


int main(int argc, char** argv)
{
    // Plus Objects
    vtkSmartPointer<vtkMatrix4x4> stylusToTracker = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkSmartPointer<vtkPlusTransformRepository> transformRepository = vtkSmartPointer<vtkPlusTransformRepository>::New();
    vtkSmartPointer<vtkPlusDataCollector> dataCollector = vtkSmartPointer<vtkPlusDataCollector>::New();
    vtkPlusChannel* trackerChannel;
    PlusTrackedFrame trackedFrame;
    PlusTransformName stylusToTrackerName;

    // Start Plus Data Collection
    std::string configFile = argv[1];
    vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(configFile.c_str()));

    // Read configuration
    if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, configFile.c_str()) == PLUS_FAIL)
    {
        LOG_ERROR("Unable to read configuration from file" << configFile.c_str());
        exit;
    }

    vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

    if (dataCollector->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
    {
        LOG_ERROR("Configuration incorrect for vtkPlusDataCollector.");
        exit;
    }
    vtkPlusDevice *trackerDevice;
    stylusToTrackerName.SetTransformName("StylusToTracker");

    // Change vtkPlusOpticalMarkerTracker to other devices as needed
    vtkSmartPointer<vtkPlusOpticalMarkerTracker> tracker = vtkSmartPointer<vtkPlusOpticalMarkerTracker>::New();

    if (dataCollector->GetDevice(trackerDevice, "TrackerDevice") != PLUS_SUCCESS)
    {
        LOG_ERROR("Unable to locate the device with ID = \"TrackerDevice\". Check config file.");
        exit;
    }

    tracker = dynamic_cast<vtkPlusOpticalMarkerTracker*>(trackerDevice);


    if (transformRepository->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
    {
        LOG_ERROR("Configuration incorrect for vtkPlusTransformRepository.");
        exit(EXIT_FAILURE);
    }

    if (tracker->GetOutputChannelByName(trackerChannel, "TrackerStream") != PLUS_SUCCESS)
    {
        LOG_ERROR("Unable to locate the channel with Id=\"TrackerStream\". Check config file.");
        exit(EXIT_FAILURE);
    }

    // Connect to devices
    if (dataCollector->Connect() != PLUS_SUCCESS)
    {
        std::cout << ".................... [FAILED]" << std::endl;
        LOG_ERROR("Failed to connect to devices!");
        exit(EXIT_FAILURE);
    }
    if (dataCollector->Start() != PLUS_SUCCESS)
    {
        LOG_ERROR("Failed to start Data collection!");
        exit(EXIT_FAILURE);
    }

    bool isValid = false;
    vtkNew<vtkTransform> tran;
    double *x;

    while (1)
    {
        trackerChannel->GetTrackedFrame(trackedFrame);
        transformRepository->SetTransforms(trackedFrame);

        if (transformRepository->GetTransform(stylusToTrackerName, stylusToTracker, &isValid) != PLUS_SUCCESS)
        {
            LOG_ERROR("Failed to get stylusToTracker Transform");
            exit;
        }
        tran->Identity();
        tran->SetMatrix(stylusToTracker);
        x = tran->GetOrientation();
        printf("Stylus To Tracker Orientation: %f %f %f\n", x[0], x[1], x[2]);
    }
}
