#include <iostream>
#include <iomanip>
#include <sstream>
#include "Pager.h"
#include "Utils.h"

// One pager for both uses. Plain viewing (selecting == false) ends with Enter; picking
// (selecting == true) returns the product whose ID the user typed.
static Product* runPager(Product** items, int count, const string& heading, const string& footer,
                         const string& emptyMessage, bool selecting) {
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

        if (count == 0 || (!selecting && pages == 1)) { pauseForEnter(); return nullptr; }   // nothing to navigate

        string prompt;
        if (selecting) {
            prompt = "\nType a product ID to select it";
            if (pages > 1) prompt += ", [n] next, [p] previous, [g#] go to page (e.g. g2)";
            prompt += ", or [Enter] to cancel: ";
        } else {
            prompt = "\n[n] next   [p] previous   [1-" + to_string(pages) + "] go to page   [Enter] continue: ";
        }
        string in = toLower(trim(readLine(prompt)));
        if (in.empty()) return nullptr;                   // done / cancelled, back to the menu

        if (in == "n" || in == "next") {
            if (page < pages) page++; else note = "  Already on the last page.";
        } else if (in == "p" || in == "prev" || in == "previous") {
            if (page > 1) page--; else note = "  Already on the first page.";
        } else if (!selecting) {
            istringstream ss(in);                         // viewing: a bare number is a page number
            int target;
            char extra;
            if ((ss >> target) && !(ss >> extra) && target >= 1 && target <= pages) page = target;
            else note = "  Enter n, p, a page number from 1 to " + to_string(pages) + ", or press Enter to continue.";
        } else {
            // picking: "g2" / "g 2" / "page 2" jump to a page, a bare number is a product ID
            bool jump = false;
            string rest;
            if (in.size() >= 4 && in.compare(0, 4, "page") == 0) { jump = true; rest = in.substr(4); }
            else if (in[0] == 'g') { jump = true; rest = in.substr(1); }

            if (jump) {
                istringstream ss(rest);
                int target;
                char extra;
                if ((ss >> target) && !(ss >> extra) && target >= 1 && target <= pages) page = target;
                else note = "  To change page type g followed by a page number from 1 to " + to_string(pages) + " (for example g2).";
            } else {
                istringstream ss(in);
                int id;
                char extra;
                if ((ss >> id) && !(ss >> extra)) {
                    for (int i = 0; i < count; i++)
                        if (items[i]->id == id) return items[i];
                    note = "  No product with ID " + to_string(id) + " in this list.";
                    if (pages > 1 && id >= 1 && id <= pages)
                        note += " (To go to page " + to_string(id) + ", type g" + to_string(id) + ".)";
                } else {
                    note = "  Type a product ID, n, p, g<page>, or press Enter to cancel.";
                }
            }
        }
    }
}

void showProductPages(Product** items, int count, const string& heading,
                      const string& footer, const string& emptyMessage) {
    runPager(items, count, heading, footer, emptyMessage, false);
}

Product* pickProductFromPages(Product** items, int count, const string& heading, const string& emptyMessage) {
    return runPager(items, count, heading, "", emptyMessage, true);
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
