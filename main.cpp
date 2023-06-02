#include "Classes.h"

void toClipboard(HWND hwnd, const string &s);

int main()
{
    HWND hwnd = GetDesktopWindow();

    char username[UNLEN+1];
    DWORD username_len = UNLEN+1;
    GetUserName(username, &username_len);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    string path = "";
    int Username_Length = 0;

    for (int i = 0; i < UNLEN+1; i++) {
        int check = username[i];

        if ((check >= 48 && check <= 57) || (check >= 65 && check <= 90) || (check >= 97 && check <= 122)) {
            Username_Length++;
        } else {
            break;
        }
    }

    for (int i = 0; i < Username_Length; i++) {
        path.append(1, username[i]);
    }

    Password_Setup* pw;
    Files* files = new Files(path.append("PW.dat"));

    char* output = "";
    int input = 0, confirm = 0;
    string website, KEY;
    while (true) {
        List* liste = files->file_Load();
        liste->quickSort(0, liste->getMax());
        pw->txtOutput(hConsole, 2, false, "=> ");
        cin >> input;

        if (!cin.fail()) {
            while (!(1 <= input <= 6)) {
                cin >> input;
            }
        } else {
            cout << "ERROR; Please use the menu with the appropriate number entries!" << endl;
            cin.clear();
            cin.sync();
        }

        switch(input) {
            case 1:
            {
                cin.ignore();
                cout << "Enter the name of the website whose password you need." << endl;

                do {
                    pw->txtOutput(hConsole, 2, false, "=> ");
                    getline(cin, website);

                    if (website.size() < 3) {
                        cout << "ERROR; Website name is too short. Try it again." << endl;
                    }
                } while(website.size() < 3);

                if (liste->search(website) == -1) {
                    cout << "ERROR; Website could not be found! Return to the menu..." << endl;
                } else {
                    bool loop = true;
                    cout << "Enter your 3-digit key in capital letters for decryption." << endl;
                    do {
                        pw->txtOutput(hConsole, 2, false, "=> ");
                        getline(cin, KEY);

                        if (KEY.size() != 3) {
                            cout << "ERROR; Invalid key length! Please try again." << endl;
                        } else {
                            loop = false;

                            for (int i = 0; i < 3; i++) {
                                int check_n = KEY[i];

                                if (check_n < 65 || check_n > 90) {
                                    cout << "ERROR; Invalid key format! Please try again." << endl;
                                    loop = true;
                                    break;
                                }
                            }
                        }
                    } while(loop);

                    output = pw->decryption(liste->getIndexPassword(liste->search(website)), liste->getIndexWebsite(liste->search(website)), KEY);

                    cout << "Your decrypted password is (Key: " << KEY << ") : ";
                    pw->txtOutput(hConsole, 11, true, string(output));

                    toClipboard(hwnd, (string) output);
                }

                website.clear();
                KEY.clear();
                break;
            }
            case 2:
            {
                cin.ignore();
                cout << "Enter the name of the website you want to generate a password for." << endl;

                do {
                    pw->txtOutput(hConsole, 2, false, "=> ");
                    getline(cin, website);

                    if (website.size() < 3) {
                        cout << "ERROR; Website name is too short. Try it again." << endl;
                    }
                } while(website.size() < 3);

                if (liste->search(website) != -1) {
                    cout << "ERROR; Website already exists! Return to the menu..." << endl;
                } else {
                    do {
                        pw = new Password_Setup(website);

                        cout << endl;
                        cout << pw->getPassword() << endl;
                        cout << "Are you happy with this generated password? (Accept with '1'; Reject with other)" << endl;

                        pw->txtOutput(hConsole, 2, false, "=> ");

                        cin >> confirm;

                        if (cin.fail()) { // neben sonstiger Zahleneingabe duerfen auch Buchstaben verwendet werden
                            cin.clear();
                            cin.sync();
                        }
                    } while (confirm != 1);

                    cin.ignore();
                    cout << "Enter a 3-digit key in capital letters for encryption." << endl;

                    do {
                        pw->txtOutput(hConsole, 2, false, "=> ");
                        getline(cin, KEY);

                        if (KEY.size() != 3) {
                            cout << "ERROR; Invalid key length! Please try again." << endl;
                        } else {
                            int i;

                            for (i = 0; i < 3; i++) {
                                int check_n = KEY[i];

                                if (check_n < 65 || check_n > 90) {
                                    cout << "ERROR; Invalid key format! Please try again." << endl;
                                    break;
                                }
                            }

                            if (i == 3) {
                                break;
                            }
                        }
                    } while(true);

                    char* encrypted = pw->encryption(KEY);

                    cout << "Your decrypted password is (KEY: " << KEY << "): ";
                    pw->txtOutput(hConsole, 11, true, string(encrypted));

                    files->file_Save(website, encrypted, liste);
                }

                website.clear();
                KEY.clear();
                break;
            }
            case 3:
            {
                Password_Setup* custom_pw = new Password_Setup();
                string passw = "";

                cin.ignore();
                cout << "Enter the name of the website for which you want to add a password." << endl;

                do {
                    pw->txtOutput(hConsole, 2, false, "=> ");
                    getline(cin, website);

                    if (website.size() < 3) {
                        cout << "ERROR; Invalid key length! Please try again." << endl;
                    }
                } while(website.size() < 3);

                if (liste->search(website) != -1) {
                    cout << "ERROR; Website already exists! Return to the menu..." << endl;
                } else {
                    cout << "Enter a default password to encrypt and store it." << endl;
                    cout << "Format: (AAAAAAA$111), (111$AAAAAAA), (AAAAAAA111$)" << endl;

                    pw->txtOutput(hConsole, 2, false, "=> ");
                    getline(cin, passw);

                    if (passw.size() != 11) {
                        cout << "ERROR; Password must be 11 characters long. Note the format! Return to the menu..." << endl;
                        break;
                    } else {
                        char* website_Upper = pw->toUpperCase(website);

                        for (int i = 0; i < 3; i++) {
                            passw.append(1, website_Upper[i]);
                        }

                        custom_pw->setPassword(passw);

                        if (custom_pw->identification()) {

                            cout << "Enter a 3-digit key in capital letters for encryption." << endl;

                            do {
                                pw->txtOutput(hConsole, 2, false, "=> ");
                                getline(cin, KEY);

                                if (KEY.size() != 3) {
                                    cout << "ERROR; Invalid key length! Please try again." << endl;
                                } else {
                                    int i;

                                    for (i = 0; i < 3; i++) {
                                        int check_n = KEY[i];

                                        if (check_n < 65 || check_n > 90) {
                                            cout << "ERROR; Invalid key format! Please try again." << endl;
                                            break;
                                        }
                                    }

                                    if (i == 3) {
                                        break;
                                    }
                                }
                            } while(true);

                            char* encrypted = custom_pw->encryption(KEY);

                            cout << "Your decrypted password is(KEY: " << KEY << ") : ";
                            pw->txtOutput(hConsole, 11, true, string(encrypted));

                            files->file_Save(website, encrypted, liste);
                        } else {
                            cout << "ERROR; Format was entered incorrectly. Return to the menu..." << endl;
                        }
                    }
                }
                break;
            }
            case 4:
            {
                liste->display();
                break;
            }
            case 5:
            {
                char* ad_pin = "";
                bool break_bool = false;

                if (liste->search(".W") == -1) {
                    int pin = 0;
                    ostringstream ostr;

                    cout << "Please create a four-digit administrator PIN." << endl;

                    do {
                        pw->txtOutput(hConsole, 2, false, "=> ");
                        cin >> pin;

                        if (cin.fail()) {
                            cout << "ERROR; PIN may only consist of digits! Try it again." << endl;
                            cin.clear();
                            cin.sync();
                        } else if (pin > 9999 || pin < 1000) {
                            cout << "Mistake; PIN must consist of exactly four digits. Try it again." << endl;
                        }
                    } while(pin > 9999 || pin < 1000);

                    ostr << pin;
                    string pinn = ostr.str();

                    for (int i = 0; i < 4; i++) {
                        ad_pin = pw->appendChar(ad_pin, pinn[i]);
                    }

                    files->file_Save(".W", ad_pin, liste);

                    cout << "Administrator PIN set successfully! Return to the menu..." << endl;
                    break;
                } else {
                    string pin = "";
                    int stelle = liste->search(".W");
                    ad_pin = liste->getIndexPassword(stelle);

                    cin.ignore();
                    cout << "Please enter your administrator PIN." << endl;

                    do {
                        pw->txtOutput(hConsole, 2, false, "=> ");
                        cin >> pin;

                        if (pin.size() != 4) {
                            cout << "ERROR; PIN must consist of four digits. Try it again." << endl;
                        } else if (pin != ad_pin) {
                            cout << "ERROR; Incorrect PIN was entered. Return to the menu..." << endl;
                            break_bool = true;
                        }
                    } while(pin.size() != 4);

                    if (break_bool) {
                        break;
                    } else {
                        cin.ignore();
                        cout << "Enter the name of the website whose password you want to delete." << endl;

                        do {
                            pw->txtOutput(hConsole, 2, false, "=> ");
                            getline(cin, website);

                            if (website.size() < 3) {
                                cout << "ERROR; Website name is too short. Try it again." << endl;
                            }
                        } while(website.size() < 3);

                        if (liste->search(website) == -1) {
                            cout << "ERROR; Website could not be found! Return to the menu..." << endl;
                        } else {
                            liste->erase(liste->search(website), files->getFILE_PATH());
                        }

                        website.clear();
                        break;
                    }
                }
            }
            case 6:
            {
                //return 0;
                exit(0);
            }
        }
    }
    return 1;
}

void toClipboard(HWND hwnd, const string &s) {
    OpenClipboard(hwnd);
    EmptyClipboard();
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);

    if (!hg) {
        CloseClipboard();
        return;
    }

    memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
    GlobalUnlock(hg);
    SetClipboardData(CF_TEXT, hg);
    CloseClipboard();
    GlobalFree(hg);
}