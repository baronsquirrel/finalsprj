#include <iostream>
#include <iomanip>
#include <sstream>
#include "Pager.h"
#include "Utils.h"

void showProductPages(Product** items, int count, const string& heading,
                      const string& footer, const string& emptyMessage) {
    int pages = (count + PRODUCTS_PER_PAGE - 1) / PRODUCTS_PER_PAGE;
    if (pages < 1) pages = 1;
    ProductTableLayout layout = makeLayout(items, count);   // measured over ALL pages, so columns stay put
    int page = 1;
    string note;            // a hint to show on the next redraw (e.g. "already on the last page")

    while (true) {
        clearScreen();
        cout << heading << "\n";

        if (count == 0) {
            cout << emptyMessage << "\n";
        } else {
            int first = (page - 1) * PRODUCTS_PER_PAGE;                 // index of the first row
            int last = first + PRODUCTS_PER_PAGE;                       // one past the last row
            if (last > count) last = count;
            printHeader(layout);
            for (int i = first; i < last; i++) printRow(*items[i], layout);
            printRule(layout);
            if (pages > 1)
                cout << "Page " << page << " of " << pages << "  (products " << first + 1
                     << "-" << last << " of " << count << ")\n";
        }
        if (!footer.empty()) cout << footer << "\n";
        if (!note.empty()) { cout << note << "\n"; note.clear(); }

        if (pages == 1) { pauseForEnter(); return; }      // nothing to navigate

        string in = toLower(trim(readLine(
            "\n[n] next   [p] previous   [1-" + to_string(pages) + "] go to page   [Enter] continue: ")));
        if (in.empty()) return;                           // done, back to the menu

        if (in == "n" || in == "next") {
            if (page < pages) page++; else note = "  Already on the last page.";
        } else if (in == "p" || in == "prev" || in == "previous") {
            if (page > 1) page--; else note = "  Already on the first page.";
        } else {
            istringstream ss(in);
            int target;
            char extra;
            if ((ss >> target) && !(ss >> extra) && target >= 1 && target <= pages) page = target;
            else note = "  Enter n, p, a page number from 1 to " + to_string(pages) + ", or press Enter to continue.";
        }
    }
}

int pickFromList(const vector<string>& options, const string& heading) {
    int count = (int)options.size();
    int pages = (count + PRODUCTS_PER_PAGE - 1) / PRODUCTS_PER_PAGE;
    if (pages < 1) pages = 1;
    int digits = (int)to_string(count).size();       // so the numbers line up
    int page = 1;
    string note;

    while (true) {
        clearScreen();
        cout << heading << "\n";

        int first = (page - 1) * PRODUCTS_PER_PAGE;
        int last = first + PRODUCTS_PER_PAGE;
        if (last > count) last = count;
        for (int i = first; i < last; i++)
            cout << "  " << right << setw(digits) << (i + 1) << ". " << options[i] << "\n";
        if (pages > 1)
            cout << "\nPage " << page << " of " << pages << "  (" << first + 1 << "-" << last
                 << " of " << count << ")\n";
        if (!note.empty()) { cout << note << "\n"; note.clear(); }

        string prompt = "\nSelect 1-" + to_string(count) + ",";
        if (pages > 1) prompt += " [n] next page, [p] previous page,";
        prompt += " or [0] back: ";
        string in = toLower(trim(readLine(prompt)));

        if (in == "0" || in == "b" || in == "back") return -1;
        if (in == "n" || in == "next") {
            if (page < pages) page++; else note = "  Already on the last page.";
        } else if (in == "p" || in == "prev" || in == "previous") {
            if (page > 1) page--; else note = "  Already on the first page.";
        } else {
            istringstream ss(in);
            int choice;
            char extra;
            if ((ss >> choice) && !(ss >> extra) && choice >= 1 && choice <= count) return choice - 1;
            note = "  Enter a number from 1 to " + to_string(count) + (pages > 1 ? ", n, p," : ",") + " or 0 to go back.";
        }
    }
}
