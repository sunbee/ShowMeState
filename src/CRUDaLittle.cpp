#include "CRUDaLittle.h"

CRUDaLittle::CRUDaLittle() {
    /*
    * Mount the file-system
    */
    if (!LittleFS.begin()) {
        Serial.println("Mounted no LittleFS!");
        return;
    }
    Serial.println("Mounted LittleFS successfully!");

}

void CRUDaLittle::readaLittle(String* file_content, const char* file_path) {
    /*
    * Read contents of the named file into a string.
    * Args:
    *   file_content is a String and passed by reference. (I.e. a pointer to String is passed.)
    *   file_path is a C string (array of char) specifying the path to file on local file-system. 
    * Result:
    *   The output, when file is read successfully, is printed to the String passed as arg.
    * Note:
    *   The implementation does not return a result. Take a reference to String and pass it as arg.
    *   The function writes the result at the location provided, making it available.
    */
    Serial.printf("Reading file: %s\r\n", file_path);

    File file = LittleFS.open(file_path, "r");
    if (!file) {
        Serial.println("Read NO file!");
        return;
    }

    Serial.print("File Contents: ");
    while (file.available()) {
        *file_content = file.readStringUntil('\n');
        break;
    }
    Serial.println(*file_content);
    file.close();
}

void CRUDaLittle::writeaLittle(const char* message, const char* file_path) {
    /*
    * Scribe a message to the named file.
    * Args: 
    *   message is a C string (i.e. array of char) containing the message.
    *   file_path is a C string specifying the path to file on local file-system.
    * Result:
    *   The text message is persisted to file, previous contents over-written.
    */
    Serial.printf("Writing file: %s\r\n", file_path);

    File file = LittleFS.open(file_path, "w");
    if (!file) {
        Serial.println("Wrote NO file!");
        return;
    }

    Serial.println(file.print(message) ? "Wrote message successfully!" : "Wrote no message!"); 
    file.close(); 
}
