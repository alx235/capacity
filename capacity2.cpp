//g++ capacity2.cpp -o capacity2 -Wall -pthread -std=c++0x
#include <iostream>
#include <string.h>
#include <vector>


namespace User {
/*COPY ON_WRITE AND REPRESENTATION VIEW
    struct Srep {

        char* s;
        int sz;//symbols count
        int n;//ref counter

        Srep(int nsz, const char* p) {
            n=1;
            sz = nsz;
            s = new char[sz+1];//add terminal-zero

            strcpy(s,p);
        }
        
        ~Srep() {delete[] s;}

        Srep* get_own_copy() {
            if (n==1) return this;
            n--;
            return new Srep(sz,s);
        }

        void assign (int nsz, const char* p) {
            if (sz!=nsz) {
                delete[] s;
                sz = nsz;
                s= new char [sz+1];
            }
            strcpy(s, p);
        }
    private://block copy and assignment operations
        Srep(const Srep&);
        Srep& operator=(const Srep&);
    };

    class String {

        //ref to String, work with s[i]
        //As for me, we can use only char* c and save memory))
        class Cref {
            friend class String;//access to private String member

            String& s;
            int i;

            Cref(String& str_ref, int i):s(str_ref),i(i) {}

            public:
            operator char() const {
                s.check(i);
                return s.read(i);
            }

            void operator= (char c) {//not check range!!!!
                s.write(i,c);
            }
        };

        Srep* rep;//representation

        void check (int i) const {
            if ((i<0) || (rep->sz<=i)) throw Range();
        }
        char read(int i) const {
            return rep->s[i];
        }
        void write (int i,char c) {
            rep = rep->get_own_copy();
            rep->s[i]=c;
        }

    public:
        //exception class
        class Range {
        public:
            Range() {
                std::cout<<"index out of range\n";
            }
        };

        String() {
            rep = new Srep(0,"");
        }
        
        String(const String& x) {
            x.rep->n++;
            rep = x.rep;
        }

        String& operator=(const String& x) {
            x.rep->n++;
            if (--rep->n==0)  delete rep;
            rep = x.rep;
            return *this;
        }

        String(const char* s) {
            rep = new Srep(strlen(s),s);
        }

        String& operator=(const char* s) {
            if (rep->n==1)
                rep->assign(strlen(s),s);
            else {
                rep->n--;
                rep = new Srep(strlen(s),s);
            }
            return *this;
        }

        Cref operator[] (int i) {
            check(i);
            return Cref(*this,i);
        }
        //for const char, maybe
        char operator[] (int i) const {
            check(i);
            return rep->s[i];
        }

        int size() const {
            return rep->sz;
        }
    };
}
*/

    struct Srep {

        char* s;
        int sz;//symbols count
        int n;//ref counter

        Srep(int nsz, const char* p) {
            n=1;
            sz = nsz;
            s = new char[sz+1];//add terminal-zero

            strcpy(s,p);
        }
        
        ~Srep() {delete[] s;}

        Srep* get_own_copy() {
            if (n==1) return this;
            n--;
            return new Srep(sz,s);
        }

        void assign (int nsz, const char* p) {
            if (sz!=nsz) {
                delete[] s;
                sz = nsz;
                s= new char [sz+1];
            }
            strcpy(s, p);
        }
    private://block copy and assignment operations
        Srep(const Srep&);
        Srep& operator=(const Srep&);
    };

    template<class C> class String {

        //ref to String, work with s[i]
        //As for me, we can use only char* c and save memory))
        class Cref {
            friend class String;//access to private String member

            String& s;
            int i;

            Cref(String& str_ref, int i):s(str_ref),i(i) {}

            public:
            operator char() const {
                s.check(i);
                return s.read(i);
            }

            void operator= (char c) {//not check range!!!!
                s.write(i,c);
            }
        };

        Srep* rep;//representation

        void check (int i) const {
            if ((i<0) || (rep->sz<=i)) throw Range();
        }
        char read(int i) const {
            return rep->s[i];
        }
        void write (int i,char c) {
            rep = rep->get_own_copy();
            rep->s[i]=c;
        }

    public:
        //exception class
        class Range {
        public:
            Range() {
                std::cout<<"index out of range\n";
            }
        };

        String() {
            rep = new Srep(0,"");
        }
        
        String(const String& x) {
            x.rep->n++;
            rep = x.rep;
        }

        String& operator=(const String& x) {
            x.rep->n++;
            if (--rep->n==0)  delete rep;
            rep = x.rep;
            return *this;
        }

        String(const C* s) {
            rep = new Srep(strlen(s),s);
        }

        String& operator=(const char* s) {
            if (rep->n==1)
                rep->assign(strlen(s),s);
            else {
                rep->n--;
                rep = new Srep(strlen(s),s);
            }
            return *this;
        }

        template <typename C1>
        friend std::ostream& operator<<(std::ostream& os, const C1& x);

        template <typename C1>
        friend std::istream& operator>>(std::istream& is, C1& x);

        Cref operator[] (int i) {
            check(i);
            return Cref(*this,i);
        }
        //for const char, maybe
        char operator[] (int i) const {
            check(i);
            return rep->s[i];
        }

        int size() const {
            return rep->sz;
        }
    };

    template <typename C>
    std::ostream& operator<<(std::ostream& os, const C& x) {
        os.write(x.rep->s,x.rep->sz);
        return os;
    }

    template <typename C>
    std::istream& operator>>(std::istream& is, C& x) {
        int size = x.rep->sz;
        is.getline(x.rep->s,size);
        return is;
    }
}


int main (int argc, char * argv[]) {

/*COPY ON_WRITE AND REPRESENTATION VIEW
    User::String str = "abc";
    char c = str[4];
*/
    User::String<char> buf;
    std::vector< User::String<char> > v;
    while(std::cin>>buf)
        v.push_back(buf);
    std::cout<<v[0]<<"\n";
}
