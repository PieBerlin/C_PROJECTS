/*Tree.c*/
#include "tree.h"
Tree root = {
    .n={
    .tag = (TagRoot | TagNode),
    .north = (struct u_node *)&root,
    .west = 0,
    .east = 0,
    .path = "/"
    }};

int8* indent(int8 n){
        static int8 buf[256];
        int8 *p;
        int16 i;

        if (n < 1 )return (int8*)"";

        // 256 /2 = 128
        assert(n < 120);
        zero(buf,256);

        for (i = 0, p = buf; i < n; i++, p+=2){
            memcpy((char *)p, "  ", 2);
        }
        return buf;
    }
/*
void print_tree(int fd, Tree *_root) {
    Node *cursor = (Node *)_root;
    int8 indentation = 0;
    int8 buf[256]; 
    int16 size;

    while (cursor != NULL) {
        // Print current line
        Print(indent(indentation));
        Print(cursor->path);
        Print("\n");

        // Move to next depth
        cursor = cursor->west;
        indentation++;

        // Safety: Prevent infinite loop if the tree is circular
        if (cursor == (Node *)_root) break; 
    }
}

*/


void print_tree(int fd,Tree *_root){

    int8 indentation;
    int8 buf[256];
    int16 size;
    Node *n;
    Leaf *l ;

    indentation = 0;

    for (n = (Node *)_root; n ;n = n ->west){
        Print(indent(indentation++));
        Print(n->path);
        printf("\n");

        if(n->east){
            for (l = n->east; l; l = l ->east ){
            

                Print(indent(indentation));
                Print(n->path);
                Print("/");
                Print(l->key);
                Print(" --> '");
                write(fd,(char*)l->value, (int)l->size);
                Print("'\n");
            }
        }

    }

    return;
    }



    void zero(int8 *str,int16 size){
        int8 *p;
        int16 n;
        for (n = 0, p = str; n<size;p++,n++){
            *p = 0;
        }
    }

    Node *create_node(Node *parent,int8 *path){
        Node *n;
        int16 size;

        errno = NoError;

        assert(parent);

        size = sizeof(struct s_node);

        n = (Node *)malloc((int)size);
        zero((int8 *)n, size);

        parent->west = n;
        n->tag = TagNode;
        n->north = (struct u_node*)parent;
        strncpy((char *)n->path, (char *)path, 254);
        return n;
    }
    Node *find_node_linear(int8* path){
        Node *p, *ret;
        for (ret = (Node* )0,p = (Node *)&root; p; p = p->west){
            if (!strcmp((char*)p->path, (char*)path)){
                ret = p;
                break;
            }
        }
        return ret;
    }

   Leaf *find_leaf_linear(int8 *path,int8 *key){
       Node *n;
       Leaf *l, *ret;

       n = find_node(path);

       if (!n){
           return (Leaf *)0;
       }

       for (ret = (Leaf *)0, l = n->east; l; l=l->east){
        if (!strcmp((char*)l->key,(char*)key)){
            ret = l;
            break;
        }
       }
       return ret;
   }

   int8 *lookup_linear(int8 *path,int8 *key){
       Leaf *p;
       p = find_leaf_linear(path, key);
       return (p) ? p->value : (int8 *)0;
   }

    Leaf *find_last_linear(Node *parent){
        Leaf *l;
        errno = NoError;
        assert(parent);
        if(!parent->east){
            reterr(NoError);
        }
        for (l = parent->east; l->east;l=l->east)
            ;
        assert(l);
        
        return l;
    }

    /*
    Leaf *create_leaf(Node *parent,int8 *key,int8 *value,int16 count){
        Leaf *l,*new;
        
        int16 size;

        assert(parent);
        //NODE -- LEAF1 --LEAF2 --LEAF3
        // we need to find the last leaf and if we dont find it wel connect to the last node
        l = find_last(parent);

        size = sizeof(struct s_leaf);
        new = (Leaf *)malloc(size);
        assert(new);


        if(!l){
            // directly connected
            parent->east = new;
        }
        else{
            // l is a leaf
            l->east = new;
        }

        zero((int8 *)new, size);
        new->tag = TagLeaf;

        new->west = (!l)?(Tree *)parent : (Tree *)l;

        strncpy((char *)new->key, (char *)key, 126);

        new->value = (int8 *)malloc(count);
        assert(new->value);
        zero(new->value, count);
        
        strncpy((char *)new->value, (char *)value, count-1);
        new->size = count;
        return new;

    } */

Leaf *create_leaf(Node *parent, int8 *key, int8 *value, int16 count) {
    Leaf *new_leaf, *last;
    int16 size = sizeof(struct s_leaf);

    assert(parent);

    new_leaf = (Leaf *)malloc(size);
    assert(new_leaf);
    zero((int8 *)new_leaf, size);

    new_leaf->tag = TagLeaf;
    strncpy((char *)new_leaf->key, (char *)key, 126);
    
    // Allocate count + 1 to ensure a null terminator fits
    new_leaf->value = (int8 *)malloc(count + 1);
    assert(new_leaf->value);
    zero(new_leaf->value, count + 1);
    memcpy(new_leaf->value, value, count);
    new_leaf->size = count;

    // Link logic
    if (!parent->east) {
        // First leaf: Node -> Leaf
        parent->east = new_leaf;
        new_leaf->west = (Tree *)parent; 
    } else {
        // Subsequent leaves: Leaf -> Leaf
        last = parent->east;
        while (last->east) {
            last = last->east;
        }
        last->east = new_leaf;
        new_leaf->west = (Tree *)last;
    }

    return new_leaf;
}


    /*

    / 
        /a
        /a/b/ 
        /a/b/c
    
    */

    Tree *example_tree(void){
        int8 c;
        Node *n,*p;
        int8 path[256];
        int32 x;

        zero(path, 256);
        
        x = 0;
        for (n = (Node *)&root, c = 'a'; c <= 'z'; c++){
            x = (int32)strlen((char *)path);
            *(path + x++) = '/';
            *(path + x) = c;

            //printf("%s\n", path);

            p  = n;

            n = create_node(p, path);

        }

        return (Tree *)&root;
    }
#if 0
    int8 *example_path(int8 path){
        static int8 buf[256];
        int8 c;

        int32 x;

        zero(buf, 256);

        for (c = 'a'; c <= path; c++){
            x = (int32)strlen((char *)buf);
            *(buf + x++) = '/';
            *(buf + x) = c;
        }

        return buf;
    }
#endif 
int8 *example_path(int8 c) {
    static int8 path_buf[512];
    int8 current;
    int32 x;

    // 1. Clear the dedicated buffer
    zero(path_buf, 512);
    
    // 2. Safety check: Only process lowercase a-z
    if (c >= 'a' && c <= 'z') {
        for (current = 'a'; current <= c; current++) {
            x = (int32)strlen((char *)path_buf);
            
            // Ensure we don't overflow the buffer
            if (x > 500) break; 

            path_buf[x++] = '/';
            path_buf[x] = current;
            path_buf[x + 1] = '\0';
        }
    } else {
        // Default to root if char is out of range
        strcpy((char *)path_buf, "/");
    }

    return path_buf;
}


int8 *example_duplicate(int8 *str) {
    static int8 dup_buf[512]; // Increased size for doubled words
    if (!str) return (int8 *)"";

    zero(dup_buf, 512);
    
    // Copy the word twice without skipping characters
    strncpy((char *)dup_buf, (char *)str, 255);
    strncat((char *)dup_buf, (char *)str, 255);

    return dup_buf;
}


int32 example_leaves() {
    FILE *fd;
    int32 y = 0;
    int8 buf[256];
    int8 *path, *val;
    Node *n;

    fd = fopen(ExampleFile, "r");
    if (!fd) return 0;

    while (fgets((char *)buf, 255, fd)) {
        // Safely remove newline
        size_t len = strlen((char *)buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = 0;
            len--;
        }
        if (len == 0) continue; // Skip empty lines

        path = example_path(buf[0]);
        n = find_node(path);
        
        if (n) {
            val = example_duplicate(buf);
            create_leaf(n, buf, val, (int16)strlen((char *)val));
            y++;
        }
        zero(buf, 256);
    }
    fclose(fd);
    return y;
}


int main(){
    Tree *example;
    int32 x;
    int8 *p;

    example = example_tree();
    printf("Populating tree ...\n");
    fflush(stdout);

    x = example_leaves();

    printf("Done. (%u)\n\n", x);

    p = lookup_linear((int8 *)"/a", (int8 *)"aal");

    if (p){
        printf("Found: %s\n", p);
    }
    else{
        printf("Not found.\n");
    }

    print_tree(1, example);



    return 0;
        }

#pragma GCC diagnostic  pop 