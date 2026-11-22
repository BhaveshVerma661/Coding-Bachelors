#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <utility>
#include <sstream>

#include "FileInfo.h"
#include "FileScanner.h"
#include "FileHasher.h"
#include "HashTable.h"
#include "Chunker.h"
#include "PerceptualHasher.h"
#include "ImageSimilarity.h"

using namespace std;

// Check image extensions
bool isImageFile(string extension)
{
    transform(extension.begin(), extension.end(),
              extension.begin(), ::tolower);

    return extension == ".jpg" ||
           extension == ".jpeg" ||
           extension == ".png" ||
           extension == ".bmp" ||
           extension == ".tif" ||
           extension == ".tiff" ||
           extension == ".webp";
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("File Deduplication System");
    window.resize(750, 550);

    // Main layout
    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *heading = new QLabel("FILE DEDUPLICATION SYSTEM");
    heading->setAlignment(Qt::AlignCenter);
    heading->setStyleSheet(
        "font-size: 22px; font-weight: bold; padding: 15px;"
    );

    QLabel *folderLabel = new QLabel("Select Folder:");

    QLineEdit *folderInput = new QLineEdit;
    folderInput->setPlaceholderText("Choose a folder to scan...");

    QPushButton *browseButton = new QPushButton("Browse");
    QPushButton *scanButton = new QPushButton("Scan Files");

    QTextEdit *results = new QTextEdit;
    results->setReadOnly(true);
    results->setPlaceholderText("Scan results will appear here...");

    // Folder selection
    QHBoxLayout *folderLayout = new QHBoxLayout;
    folderLayout->addWidget(folderInput);
    folderLayout->addWidget(browseButton);

    // Add widgets
    layout->addWidget(heading);
    layout->addWidget(folderLabel);
    layout->addLayout(folderLayout);
    layout->addWidget(scanButton);
    layout->addWidget(results);

    window.setLayout(layout);

    // Browse button
    QObject::connect(browseButton, &QPushButton::clicked, [&]()
    {
        QString folder = QFileDialog::getExistingDirectory(
            &window, "Select Folder"
        );

        if (!folder.isEmpty())
            folderInput->setText(folder);
    });

    // Scan button
    QObject::connect(scanButton, &QPushButton::clicked, [&]()
    {
        QString folder = folderInput->text();

        if (folder.isEmpty())
        {
            QMessageBox::warning(
                &window, "Error", "Please select a folder first."
            );
            return;
        }

        scanButton->setEnabled(false);
        results->setPlainText("Scanning files...");

        QApplication::processEvents();

        ostringstream output;
        streambuf *oldBuffer = cout.rdbuf(output.rdbuf());

        string folderPath = folder.toStdString();

        // Scan folder
        FileScanner scanner;
        vector<FileInfo> files = scanner.scanFolder(folderPath);

        cout << "FILE DEDUPLICATION REPORT\n";
        cout << "Folder: " << folderPath << "\n\n";

        if (files.empty())
        {
            cout << "No files found.\n";
        }
        else
        {
            cout << "Total files found: "
                 << files.size() << "\n";

            // Exact duplicate detection
            cout << "\n========== EXACT DUPLICATES ==========\n";

            HashTable duplicateTable;

            for (FileInfo& file : files)
            {
                string hash;

                if (FileHasher::calculateHash(file.getPath(), hash))
                {
                    file.setHash(hash);
                    duplicateTable.insert(hash, file.getPath());
                }
            }

            duplicateTable.displayDuplicates();

            // Image similarity detection
            cout << "\n========== IMAGE SIMILARITY ==========\n";

            vector<pair<string, unsigned long long>> imageHashes;

            for (FileInfo& file : files)
            {
                if (!isImageFile(file.getExtension()))
                    continue;

                unsigned long long hash = 0;

                if (PerceptualHasher::calculateHash(
                        file.getPath(), hash))
                {
                    imageHashes.push_back(
                        {file.getPath(), hash}
                    );
                }
            }

            cout << "Total images processed: "
                 << imageHashes.size() << "\n";

            bool found = false;

            for (size_t i = 0; i < imageHashes.size(); i++)
            {
                for (size_t j = i + 1; j < imageHashes.size(); j++)
                {
                    int distance =
                        ImageSimilarity::calculateHammingDistance(
                            imageHashes[i].second,
                            imageHashes[j].second
                        );

                    if (ImageSimilarity::areSimilar(
                            imageHashes[i].second,
                            imageHashes[j].second))
                    {
                        cout << "\nSimilar Images:\n";
                        cout << imageHashes[i].first << "\n";
                        cout << imageHashes[j].first << "\n";
                        cout << "Hamming Distance: "
                             << distance << "\n";

                        found = true;
                    }
                }
            }

            if (!found)
                cout << "No similar images found.\n";

            // Chunk similarity
            cout << "\n========== CHUNK SIMILARITY ==========\n";

            vector<string> filePaths;

            for (FileInfo& file : files)
                filePaths.push_back(file.getPath());

            Chunker chunker;
            chunker.findChunks(filePaths);
        }

        // Restore console output
        cout.rdbuf(oldBuffer);

        // Display results
        QString reportText = QString::fromStdString(output.str());
        results->setPlainText(reportText);

        // Save report in selected folder
        QString reportPath = QDir(folder).filePath(
            "Deduplication_Report.txt"
        );

        QFile reportFile(reportPath);

        if (reportFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            reportFile.write(reportText.toUtf8());
            reportFile.close();

            QMessageBox::information(
                &window,
                "Completed",
                "Scanning completed!\n\nReport saved at:\n" + reportPath
            );
        }
        else
        {
            QMessageBox::warning(
                &window, "Error", "Could not create the report."
            );
        }

        scanButton->setEnabled(true);
    });

    window.show();

    return app.exec();
}