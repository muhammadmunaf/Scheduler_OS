template <class T>
class Node
{
public:
    Node* next;
    T data;

    Node()
    {
        next = NULL;
    }
};

template <class T>
class LinkedList
{
public:
    Node<T>* head;
    Node<T>* current;

    LinkedList()
    {
        head = NULL;
        current = NULL;
    }

    void insert(T dataItem)
    {
        Node<T>* temp = head;
        if (head == NULL)
        {
            temp = new Node<T>;
            temp->data = dataItem;
            temp->next = NULL;
            current = temp;
            head = temp;
        }
        else if (head != NULL)
        {
            while (1)
            {
                if (temp->next == NULL)
                {
                    temp->next = new Node<T>;
                    temp = temp->next;
                    temp->data = dataItem;
                    temp->next = NULL;
                    break;
                }
                temp = temp->next;
            }
        }
    }

    void print()
    {
        Node<T>* temp = head;
        while (1)
        {
            if (temp == NULL)
                break;

            cout << temp->data << " ";

            temp = temp->next;
        }
        cout << endl;
    }

    bool isEmpty()
    {
        if (head == NULL)
            return 1;
        else
            return 0;
    }

    void pop()
    {
        head = head->next;
    }
};
