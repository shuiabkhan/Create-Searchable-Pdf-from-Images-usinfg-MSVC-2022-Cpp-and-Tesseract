#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <tesseract/renderer.h>
#include <filesystem> //c++ 17
#include <iostream>
#include <string>

namespace fs = std::filesystem;

int processFolder(const std::string& inputFolderPath, const std::string& outputPdfPath, tesseract::TessBaseAPI* api) {
    // Create PDF renderer
    tesseract::TessPDFRenderer* renderer = new tesseract::TessPDFRenderer(
        outputPdfPath.c_str(), api->GetDatapath(), false);

    // Check if renderer is properly initialized
    if (!renderer) {
        std::cerr << "Could not initialize PDF renderer." << std::endl;
        return EXIT_FAILURE;
    }

    // Loop through each file in the directory
    for (const auto& entry : fs::directory_iterator(inputFolderPath)) {
        if (entry.is_regular_file()) {
            std::string imagePath = entry.path().string();
            std::cout << "Processing: " << imagePath << std::endl;

            // Read the image using Leptonica
            Pix* image = pixRead(imagePath.c_str());
            if (!image) {
                std::cerr << "Could not read input image: " << imagePath << std::endl;
                continue;
            }

            api->SetImage(image);

            // Process the image and add it to the PDF
            bool succeed = api->ProcessPages(imagePath.c_str(), nullptr, 0, renderer);
            if (!succeed) {
                std::cerr << "Error during processing: " << imagePath << std::endl;
            }

            // Clean up
            pixDestroy(&image);
        }
    }

    // Finalize and close the PDF
    delete renderer;
    return EXIT_SUCCESS;
}

int main()
{
    char* outText;
    const std::string inputFolderPath = "E:\\100001"; //Imput Image Folder Path
    const std::string outputPdfPath = "E:\\100001\\output.pdf"; //Pdf Output Path

    // Initialize Tesseract API with English and Hindi languages
    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    if (api->Init(NULL, "eng+hin")) {  // Use "eng+hin" for English and Hindi
        std::cerr << "Could not initialize tesseract." << std::endl;
        return EXIT_FAILURE;
    }
    // Process the folder and create a single PDF
    if (processFolder(inputFolderPath, outputPdfPath, api) == EXIT_SUCCESS) {
        std::cout << "PDF generated successfully: " << outputPdfPath << std::endl;
    }
    else {
        std::cerr << "Failed to generate PDF." << std::endl;
    }

    // Clean up
    api->End();
    delete api;

    return EXIT_SUCCESS;
}