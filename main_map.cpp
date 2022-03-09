#include <iostream>
#include <functional>
#include <cstddef>
#include <fstream>
#include <cstring>
#include "utility.hpp"
#include "exceptions.hpp"
#include "string.hpp"

namespace sjtu {
    template<class Key, class T, int M = 16, class Compare= std::less<Key> >
    class bpt {
    public:
        typedef pair<Key, T> value_type;
        struct node {
            bool is_leave = 0;
            int length = 0;
            long long address = 0;
            long long father = -1;
            value_type value[M];
            long long son[M];
            long long pre = -1;
            long long next = -1;
        };
        node root;
        std::fstream file;
        /*std::fstream file_index;
        std::fstream file_delete;*/
        std::string file_name;
        /*std::string file_index_name;
        std::string file_delete_name;*/
        Compare cpy;

        explicit bpt(std::string file_name_/*, std::string file_index_name_, std::string file_delete_name_*/) {
            file_name = file_name_;
            /*file_index_name = file_index_name_;
            file_delete_name = file_delete_name_;*/
            file.open(file_name);
            if (!file) {
                file.open(file_name, std::fstream::out);
                file.close();
                file.open(file_name);
            }
            node tmp;
            file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
            file.close();
        }

        /*~bpt() {
            remove(reinterpret_cast<char *>(&file_name));
        }*/

        void insert(const value_type &value) {
            file.open(file_name);
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            if (!root.length) {
                node tmp;
                file.seekg(0, std::ios::end);
                tmp.address = file.tellg();
                root.son[0] = tmp.address;
                tmp.father = 0;
                tmp.is_leave = 1;
                root.length++;
                tmp.length++;
                tmp.value[0] = value;
                /*tmp.value[0].first = value.first;
                tmp.value[0].second = value.second;*/
                file.seekp(tmp.address);
                file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                file.seekp(0);
                file.write(reinterpret_cast<char *>(&root), sizeof(node));
                file.close();
                return;
            }
            node now = root;
            while (1) {
                int i;
                for (i = 0; i < now.length - 1; i++)
                    if (cpy(value.first, now.value[i].first))
                        break;
                file.seekg(now.son[i]);
                node tmp;
                file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                value_type v_up;
                if (tmp.is_leave) {
                    int i;
                    for (i = 0; i < tmp.length; i++) {
                        if (!cpy(value.first, tmp.value[i].first) && !cpy(tmp.value[i].first, value.first)) {
                            file.close();
                            throw int();
                        }
                        if (cpy(value.first, tmp.value[i].first)) {
                            for (int j = tmp.length - 1; j >= i; j--)
                                tmp.value[j + 1] = tmp.value[j];
                            tmp.value[i] = value;
                            break;
                        }
                    }
                    if (i == tmp.length)
                        tmp.value[tmp.length] = value;
                    tmp.length++;
                    file.seekp(tmp.address);
                    file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                    if (tmp.length == M) {
                        tmp.length = M >> 1;
                        file.seekp(0, std::fstream::end);
                        node new_tmp;
                        new_tmp.address = file.tellp();//新取一个地址 可记录
                        new_tmp.father = now.address;
                        if (tmp.next != -1) {
                            file.seekg(tmp.next);
                            node tmp_0;
                            file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                            tmp_0.pre = new_tmp.address;
                            new_tmp.next = tmp_0.address;
                            file.seekp(tmp_0.address);
                            file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                        }
                        tmp.next = new_tmp.address;
                        new_tmp.pre = tmp.address;
                        file.seekp(tmp.address);
                        file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                        for (int i = 0; i < M - tmp.length; i++)
                            new_tmp.value[i] = tmp.value[i + tmp.length];
                        new_tmp.length = M - tmp.length;
                        new_tmp.is_leave = 1;
                        file.seekp(new_tmp.address);
                        file.write(reinterpret_cast<char *>(&new_tmp), sizeof(node));
                        v_up = new_tmp.value[0];
                        bool root_ = 1;
                        node node_back = new_tmp;
                        while (node_back.father) {
                            root_ = 0;
                            file.seekg(node_back.father);
                            file.read(reinterpret_cast<char *>(&node_back), sizeof(node));
                            //node_back = node_back.father;
                            int i;
                            for (i = 0; i < node_back.length - 1; i++) {
                                if (cpy(new_tmp.value[0].first, node_back.value[i].first)) {
                                    for (int j = node_back.length - 1; j >= i; j--) {
                                        node_back.value[j + 1] = node_back.value[j];
                                        node_back.son[j + 1] = node_back.son[j];
                                    }
                                    node_back.value[i] = v_up;
                                    node_back.son[i + 1] = new_tmp.address;//!
                                    break;
                                }
                            }
                            if (i + 1 == node_back.length) {
                                node_back.value[node_back.length - 1] = v_up;
                                node_back.son[node_back.length] = new_tmp.address;
                            }
                            node_back.length++;
                            file.seekp(node_back.address);
                            file.write(reinterpret_cast<char *>(&node_back), sizeof(node));
                            if (node_back.length == M) {
                                if (!node_back.father) root_ = 1;
                                node_back.length = M >> 1;
                                file.seekp(node_back.address);
                                file.write(reinterpret_cast<char *>(&node_back), sizeof(node));
                                for (int i = 0; i < M - node_back.length - 1; i++) {
                                    new_tmp.value[i] = node_back.value[i + node_back.length];
                                    new_tmp.son[i] = node_back.son[i + node_back.length];
                                }
                                v_up = node_back.value[node_back.length - 1];
                                new_tmp.length = M - node_back.length;
                                new_tmp.son[new_tmp.length - 1] = node_back.son[M - 1];
                                file.seekg(0, std::fstream::end);
                                new_tmp.address = file.tellg();//新取一个地址 可记录
                                new_tmp.father = node_back.father;//!!
                                new_tmp.is_leave = 0;
                                file.write(reinterpret_cast<char *>(&new_tmp), sizeof(node));
                                node tmp_;
                                for (int i = 0; i < new_tmp.length; i++) {
                                    file.seekg(new_tmp.son[i]);
                                    file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                                    tmp_.father = new_tmp.address;
                                    file.seekp(new_tmp.son[i]);
                                    file.write(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                                }
                                //file.seekg(node_back.father);
                                //file.read(reinterpret_cast<char *>(&node_back), sizeof(node));
                                //node_back = node_back.father;
                            } else break;
                        }
                        if (!node_back.father && root_) {
                            file.seekg(node_back.father);
                            file.read(reinterpret_cast<char *>(&node_back), sizeof(node));
                            int i;
                            for (i = 0; i < node_back.length - 1; i++) {
                                if (cpy(new_tmp.value[0].first, node_back.value[i].first)) {
                                    for (int j = node_back.length - 1; j >= i; j--) {
                                        node_back.value[j + 1] = node_back.value[j];//第一个无用移动
                                        node_back.son[j + 1] = node_back.son[j];//最后一个无效移动
                                    }
                                    node_back.value[i] = v_up;
                                    node_back.son[i + 1] = new_tmp.address;//!
                                    break;
                                }
                            }
                            if (i + 1 == node_back.length) {
                                node_back.value[node_back.length - 1] = v_up;
                                node_back.son[node_back.length] = new_tmp.address;
                            }
                            node_back.length++;
                            file.seekp(node_back.address);
                            file.write(reinterpret_cast<char *>(&node_back), sizeof(node));
                            if (node_back.length == M) {
                                node_back.length = M >> 1;
                                file.seekp(node_back.address);
                                file.write(reinterpret_cast<char *>(&node_back), sizeof(node));
                                for (int i = 0; i < M - node_back.length - 1; i++) {
                                    new_tmp.value[i] = node_back.value[i + node_back.length];
                                    new_tmp.son[i] = node_back.son[i + node_back.length];
                                }
                                v_up = node_back.value[node_back.length - 1];
                                new_tmp.length = M - node_back.length;
                                new_tmp.son[new_tmp.length - 1] = node_back.son[M - 1];
                                file.seekg(0, std::fstream::end);
                                new_tmp.address = file.tellg();//新取一个地址 可记录
                                new_tmp.father = 0;
                                new_tmp.is_leave = 0;
                                file.write(reinterpret_cast<char *>(&new_tmp), sizeof(node));
                                node tmp_;
                                for (int i = 0; i < new_tmp.length; i++) {
                                    file.seekg(new_tmp.son[i]);
                                    file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                                    tmp_.father = new_tmp.address;
                                    file.seekp(new_tmp.son[i]);
                                    file.write(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                                }
                                file.seekg(0, std::fstream::end);
                                node_back.address = file.tellg();//新取一个地址 可记录
                                node_back.father = 0;
                                file.write(reinterpret_cast<char *>(&node_back), sizeof(node));
                                for (int i = 0; i < node_back.length; i++) {
                                    file.seekg(node_back.son[i]);
                                    file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                                    tmp_.father = node_back.address;
                                    file.seekp(node_back.son[i]);
                                    file.write(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                                }
                                node root_;
                                root_.length = 2;
                                root_.son[0] = node_back.address;
                                root_.son[1] = new_tmp.address;
                                root_.value[0] = v_up;
                                file.seekp(0, std::fstream::beg);
                                file.write(reinterpret_cast<char *>(&root_), sizeof(node));
                            }
                        }
                    }
                    break;
                } else
                    now = tmp;
            }
            file.close();
        }

        T find(const Key &key) {
            file.open(file_name);
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            node now = root;
            while (!now.is_leave) {
                int i;
                for (i = 0; i < now.length - 1; i++)
                    if (cpy(key, now.value[i].first))
                        break;
                file.seekg(now.son[i]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            for (int i = 0; i < now.length; i++)
                if (!cpy(key, now.value[i].first) && !cpy(now.value[i].first, key)) {
                    file.close();
                    return now.value[i].second;
                }
            file.close();
            throw int();
        }

        void erase(const Key &key) {
            file.open(file_name);
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            if (!root.length) {
                file.close();
                throw int();
            }
            node now = root;
            while (!now.is_leave) {
                int i;
                for (i = 0; i < now.length - 1; i++)
                    if (cpy(key, now.value[i].first))
                        break;
                file.seekg(now.son[i]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            for (int i = 0; i < now.length; i++)
                if (!cpy(key, now.value[i].first) && !cpy(now.value[i].first, key)) {
                    value_type v_up = now.value[0];
                    for (int j = i; j < now.length - 1; j++)
                        now.value[j] = now.value[j + 1];
                    now.length--;
                    file.seekp(now.address);
                    file.write(reinterpret_cast<char *>(&now), sizeof(node));
                    if (!i) {
                        node tmp = now;
                        bool ok = 0;
                        while (tmp.father != -1) {
                            file.seekg(tmp.father);
                            file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                            for (int j = 0; j < tmp.length - 1; j++) {
                                if (!cpy(v_up.first, tmp.value[j].first) && !cpy(tmp.value[j].first, v_up.first)) {
                                    tmp.value[j] = now.value[0];
                                    file.seekp(tmp.address);
                                    file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                    ok = 1;
                                    break;
                                }
                            }
                            if (ok) break;
                        }
                    }
                    if (now.length <= (M - 2) / 2) {
                        node tmp;
                        if (now.pre != -1) {
                            file.seekg(now.pre);
                            file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                            if (tmp.length > (M) / 2) {
                                now.length++;
                                tmp.length--;
                                for (int k = now.length - 1; k >= 0; k--)
                                    now.value[k + 1] = now.value[k];
                                now.value[0] = tmp.value[tmp.length];
                                file.seekp(tmp.address);
                                file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                file.seekp(now.address);
                                file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                file.seekg(now.father);
                                file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                                bool ok = 0;
                                while (1) {
                                    for (int k = 0; k < tmp.length - 1; k++) {
                                        if (!cpy(tmp.value[k].first, v_up.first) &&
                                            !cpy(v_up.first, tmp.value[k].first)) {
                                            tmp.value[k] = now.value[0];
                                            file.seekp(tmp.address);
                                            file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                            ok = 1;
                                            break;
                                        }
                                    }
                                    if (ok) break;
                                    else {
                                        file.seekg(tmp.father);
                                        file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                                    }
                                }
                                file.close();
                                return;
                            }
                        }
                        if (now.next != -1) {
                            file.seekg(now.next);
                            file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                            if (tmp.length > (M) / 2) {
                                now.length++;
                                tmp.length--;
                                now.value[now.length - 1] = tmp.value[0];
                                value_type up = tmp.value[0];
                                for (int k = 0; k <= tmp.length - 1; k++)
                                    tmp.value[k] = tmp.value[k + 1];
                                file.seekp(tmp.address);
                                file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                file.seekp(now.address);
                                file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                file.seekg(now.father);
                                file.read(reinterpret_cast<char *>(&now), sizeof(node));
                                bool ok = 0;
                                while (1) {
                                    for (int k = 0; k < now.length - 1; k++) {
                                        if (!cpy(now.value[k].first, up.first) &&
                                            !cpy(up.first, now.value[k].first)) {
                                            now.value[k] = tmp.value[0];
                                            file.seekp(now.address);
                                            file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                            ok = 1;
                                            break;
                                        }
                                    }
                                    if (ok) break;
                                    else {
                                        file.seekg(now.father);
                                        file.read(reinterpret_cast<char *>(&now), sizeof(node));
                                    }
                                }
                                file.close();
                                return;
                            }
                        }
                        if (now.pre != -1 || now.next != -1) {
                            if (now.pre != -1) {
                                file.seekg(now.pre);
                                file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                                if (now.father == tmp.father) {
                                    for (int k = 0; k < now.length; k++) {
                                        tmp.value[k + tmp.length] = now.value[k];
                                    }
                                    tmp.length += now.length;
                                    tmp.next = now.next;
                                    file.seekp(tmp.address);
                                    file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                    if (tmp.next != -1) {
                                        file.seekg(tmp.next);
                                        node tmp_;
                                        file.read(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                        tmp_.pre = tmp.address;
                                        file.seekp(tmp_.address);
                                        file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                    }
                                    file.seekg(now.father);
                                    file.read(reinterpret_cast<char *>(&now), sizeof(node));
                                    for (int k = 0; k < now.length - 1; k++) {
                                        if (!cpy(v_up.first, now.value[k].first) &&
                                            !cpy(now.value[k].first, v_up.first)) {
                                            for (int u = k; u < now.length - 2; u++) {
                                                now.value[u] = now.value[u + 1];
                                                now.son[u + 1] = now.son[u + 2];
                                            }
                                            break;
                                        }
                                    }
                                    now.length--;
                                    file.seekp(now.address);
                                    file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                    //bool root_ = 0;
                                    while (now.father != -1) {
                                        if (now.length <= (M - 2) / 2) {
                                            file.seekg(now.father);
                                            file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                                            int k;
                                            for (k = 0; k < tmp.length - 1; k++) {
                                                if (cpy(now.value[0].first, tmp.value[k].first))
                                                    break;
                                            }
                                            if (k != tmp.length - 1) {
                                                file.seekg(tmp.son[k + 1]);
                                                node tmp_;
                                                file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                                                if (tmp_.length > (M) / 2) {
                                                    now.length++;
                                                    now.value[now.length - 2] = tmp.value[k];
                                                    now.son[now.length - 1] = tmp_.son[k];
                                                    file.seekg(now.son[now.length - 1]);
                                                    node tmp_0;
                                                    file.read(reinterpret_cast<char *>(&tmp_0), sizeof(tmp_0));
                                                    tmp_0.father = now.address;
                                                    file.seekp(now.son[now.length - 1]);
                                                    file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                                    tmp.value[k] = tmp_.value[0];
                                                    for (int u = 0; u < tmp_.length - 2; u++) {
                                                        tmp_.value[u] = tmp_.value[u + 1];
                                                        tmp_.son[u] = tmp_.son[u + 1];
                                                    }
                                                    tmp_.son[tmp_.length - 2] = tmp_.son[tmp_.length - 1];
                                                    tmp_.length--;
                                                    file.seekp(now.address);
                                                    file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                                    file.seekp(tmp.address);
                                                    file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                                    file.seekp(tmp_.address);
                                                    file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                                    break;
                                                } else {
                                                    now.value[now.length - 1] = tmp.value[k];
                                                    for (int u = 0; u < tmp_.length - 1; u++) {
                                                        now.value[u + now.length] = tmp_.value[u];
                                                        now.son[u + now.length] = tmp_.son[u];
                                                    }
                                                    now.length += tmp_.length;
                                                    now.son[now.length - 1] = tmp_.son[tmp_.length - 1];
                                                    file.seekp(now.address);
                                                    file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                                    for (int u = 0; u <= tmp_.length - 1; u++) {
                                                        file.seekg(now.son[u + now.length - tmp_.length]);
                                                        node tmp_0;
                                                        file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                                        tmp_0.father = now.address;
                                                        file.seekp(now.son[u + now.length - tmp_.length]);
                                                        file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                                    }
                                                    for (int u = k; u < tmp.length - 2; u++) {
                                                        tmp.value[u] = tmp.value[u + 1];
                                                        tmp.son[u + 1] = tmp.son[u + 2];
                                                    }
                                                    tmp.length--;
                                                    file.seekp(tmp.address);
                                                    file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                                }
                                            } else {
                                                file.seekg(tmp.son[k - 1]);
                                                node tmp_;
                                                file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                                                if (tmp_.length > (M) / 2) {
                                                    now.son[now.length] = now.son[now.length - 1];
                                                    for (int u = now.length - 1; u >= 1; u--) {
                                                        now.son[u] = now.son[u - 1];
                                                        now.value[u] = now.value[u - 1];
                                                    }
                                                    ++now.length;
                                                    now.value[0] = tmp.value[tmp.length - 2];
                                                    now.son[0] = tmp_.son[tmp_.length - 1];
                                                    file.seekg(now.son[0]);
                                                    node tmp_0;
                                                    file.read(reinterpret_cast<char *>(&tmp_0), sizeof(tmp_0));
                                                    tmp_0.father = now.address;
                                                    file.seekp(now.son[0]);
                                                    file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                                    tmp.value[tmp.length - 2] = tmp_.value[tmp_.length - 2];
                                                    tmp_.length--;
                                                    file.seekp(now.address);
                                                    file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                                    file.seekp(tmp.address);
                                                    file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                                    file.seekp(tmp_.address);
                                                    file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                                    break;
                                                } else {
                                                    tmp_.value[tmp_.length - 1] = tmp.value[tmp.length - 2];
                                                    for (int u = 0; u < now.length - 1; u++) {
                                                        tmp_.value[u + tmp_.length] = now.value[u];
                                                        tmp_.son[u + tmp_.length] = now.son[u];
                                                    }
                                                    tmp_.length += now.length;
                                                    tmp_.son[tmp_.length - 1] = now.son[now.length - 1];
                                                    file.seekp(tmp_.address);
                                                    file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                                    for (int u = 0; u <= now.length - 1; u++) {
                                                        file.seekg(tmp_.son[u + tmp_.length - now.length]);
                                                        node tmp_0;
                                                        file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                                        tmp_0.father = tmp_.address;
                                                        file.seekp(tmp_.son[u + tmp_.length - now.length]);
                                                        file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                                    }
                                                    tmp.length--;
                                                    file.seekp(tmp.address);
                                                    file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                                }
                                                now = tmp_;
                                            }
                                            file.seekg(now.father);
                                            file.read(reinterpret_cast<char *>(&now), sizeof(node));
                                        } else break;
                                    }
                                    if (now.father == -1) {
                                        if (now.length == 1) {
                                            file.seekg(now.son[0]);
                                            file.read(reinterpret_cast<char *>(&now), sizeof(node));
                                            if (now.is_leave) {
                                                file.close();
                                                return;
                                            }
                                            now.address = 0;
                                            now.father = -1;
                                            file.seekp(0);
                                            file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                            for (int i = 0; i < now.length; i++) {
                                                node tmp_0;
                                                file.seekg(now.son[i]);
                                                file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                                tmp_0.father = 0;
                                                file.seekp(now.son[i]);
                                                file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                            }
                                        }
                                    }
                                    file.close();
                                    return;
                                }
                            }
                            file.seekg(now.next);
                            file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                            for (int k = 0; k < tmp.length; k++) {
                                now.value[k + now.length] = tmp.value[k];
                            }
                            now.length += tmp.length;
                            now.next = tmp.next;
                            v_up = tmp.value[0];
                            file.seekp(now.address);
                            file.write(reinterpret_cast<char *>(&now), sizeof(node));
                            if (now.next != -1) {
                                file.seekg(now.next);
                                node tmp_;
                                file.read(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                tmp_.pre = now.address;
                                file.seekp(tmp_.address);
                                file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                            }
                            file.seekg(now.father);
                            file.read(reinterpret_cast<char *>(&now), sizeof(node));
                            for (int k = 0; k < now.length - 1; k++) {
                                if (!cpy(v_up.first, now.value[k].first) &&
                                    !cpy(now.value[k].first, v_up.first)) {
                                    for (int u = k; u < now.length - 2; u++) {
                                        now.value[u] = now.value[u + 1];
                                        now.son[u + 1] = now.son[u + 2];
                                    }
                                    break;
                                }
                            }
                            now.length--;
                            file.seekp(now.address);
                            file.write(reinterpret_cast<char *>(&now), sizeof(node));
                            while (now.father != -1) {
                                if (now.length <= (M - 2) / 2) {
                                    file.seekg(now.father);
                                    file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                                    int k;
                                    for (k = 0; k < tmp.length - 1; k++) {
                                        if (cpy(now.value[0].first, tmp.value[k].first))
                                            break;
                                    }
                                    if (k != tmp.length - 1) {
                                        file.seekg(tmp.son[k + 1]);
                                        node tmp_;
                                        file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                                        if (tmp_.length > (M) / 2) {
                                            now.length++;
                                            now.value[now.length - 2] = tmp.value[k];
                                            now.son[now.length - 1] = tmp_.son[k];
                                            file.seekg(now.son[now.length - 1]);
                                            node tmp_0;
                                            file.read(reinterpret_cast<char *>(&tmp_0), sizeof(tmp_0));
                                            tmp_0.father = now.address;
                                            file.seekp(now.son[now.length - 1]);
                                            file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                            tmp.value[k] = tmp_.value[0];
                                            for (int u = 0; u < tmp_.length - 2; u++) {
                                                tmp_.value[u] = tmp_.value[u + 1];
                                                tmp_.son[u] = tmp_.son[u + 1];
                                            }
                                            tmp_.son[tmp_.length - 2] = tmp_.son[tmp_.length - 1];
                                            tmp_.length--;
                                            file.seekp(now.address);
                                            file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                            file.seekp(tmp.address);
                                            file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                            file.seekp(tmp_.address);
                                            file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                            break;
                                        } else {
                                            now.value[now.length - 1] = tmp.value[k];
                                            for (int u = 0; u < tmp_.length - 1; u++) {
                                                now.value[u + now.length] = tmp_.value[u];
                                                now.son[u + now.length] = tmp_.son[u];
                                            }
                                            now.length += tmp_.length;
                                            now.son[now.length - 1] = tmp_.son[tmp_.length - 1];
                                            file.seekp(now.address);
                                            file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                            for (int u = 0; u <= tmp_.length - 1; u++) {
                                                file.seekg(now.son[u + now.length - tmp_.length]);
                                                node tmp_0;
                                                file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                                tmp_0.father = now.address;
                                                file.seekp(now.son[u + now.length - tmp_.length]);
                                                file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                            }
                                            for (int u = k; u < tmp.length - 2; u++) {
                                                tmp.value[u] = tmp.value[u + 1];
                                                tmp.son[u + 1] = tmp.son[u + 2];
                                            }
                                            tmp.length--;
                                            file.seekp(tmp.address);
                                            file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                        }
                                    } else {
                                        file.seekg(tmp.son[k - 1]);
                                        node tmp_;
                                        file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                                        if (tmp_.length > (M) / 2) {
                                            now.son[now.length] = now.son[now.length - 1];
                                            for (int u = now.length - 1; u >= 1; u--) {
                                                now.son[u] = now.son[u - 1];
                                                now.value[u] = now.value[u - 1];
                                            }
                                            ++now.length;
                                            now.value[0] = tmp.value[tmp.length - 2];
                                            now.son[0] = tmp_.son[tmp_.length - 1];
                                            file.seekg(now.son[0]);
                                            node tmp_0;
                                            file.read(reinterpret_cast<char *>(&tmp_0), sizeof(tmp_0));
                                            tmp_0.father = now.address;
                                            file.seekp(now.son[0]);
                                            file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                            tmp.value[tmp.length - 2] = tmp_.value[tmp_.length - 2];
                                            tmp_.length--;
                                            file.seekp(now.address);
                                            file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                            file.seekp(tmp.address);
                                            file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                            file.seekp(tmp_.address);
                                            file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                            break;
                                        } else {
                                            tmp_.value[tmp_.length - 1] = tmp.value[tmp.length - 2];
                                            for (int u = 0; u < now.length - 1; u++) {
                                                tmp_.value[u + tmp_.length] = now.value[u];
                                                tmp_.son[u + tmp_.length] = now.son[u];
                                            }
                                            tmp_.length += now.length;
                                            tmp_.son[tmp_.length - 1] = now.son[now.length - 1];
                                            file.seekp(tmp_.address);
                                            file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                            for (int u = 0; u <= now.length - 1; u++) {
                                                file.seekg(tmp_.son[u + tmp_.length - now.length]);
                                                node tmp_0;
                                                file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                                tmp_0.father = tmp_.address;
                                                file.seekp(tmp_.son[u + tmp_.length - now.length]);
                                                file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                            }
                                            tmp.length--;
                                            file.seekp(tmp.address);
                                            file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                        }
                                        now = tmp_;
                                    }
                                    file.seekg(now.father);
                                    file.read(reinterpret_cast<char *>(&now), sizeof(node));
                                } else break;
                            }
                            if (now.father == -1) {
                                if (now.length == 1) {
                                    file.seekg(now.son[0]);
                                    file.read(reinterpret_cast<char *>(&now), sizeof(node));
                                    if (now.is_leave) {
                                        file.close();
                                        return;
                                    }
                                    now.address = 0;
                                    now.father = -1;
                                    file.seekp(0);
                                    file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                    for (int i = 0; i < now.length; i++) {
                                        node tmp_0;
                                        file.seekg(now.son[i]);
                                        file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                        tmp_0.father = 0;
                                        file.seekp(now.son[i]);
                                        file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                    }
                                }
                            }
                            file.close();
                            return;
                        }
                        if (!now.length) {
                            root.length--;
                            file.seekp(0);
                            file.write(reinterpret_cast<char *>(&root), sizeof(node));
                        }
                    }
                    file.close();
                    return;
                }
            file.close();
            throw int();
        }
    };
}
sjtu::bpt<sjtu::string, int, 4> tree("file");

int main() {
    int n;
    std::cin >> n;
    for (int i = 1; i <= n; i++) {
        sjtu::string s;
        std::cin >> s;
        if (s == "insert") {
            int x;
            std::cin >> s;
            std::cin >> x;
            try { tree.insert(sjtu::pair<sjtu::string, int>(s, x)); }
            catch (...) {}
        } else if (s == "find") {
            std::cin >> s;
            try { std::cout << tree.find(s) << std::endl; }
            catch (...) { std::cout << "NULL" << std::endl; }
        } else if (s == "delete") {
            int x;
            std::cin >> s;
            //std::cin >> x;
            //try { tree.erase(sjtu::pair<sjtu::string, int>(s, x)); }
            try { tree.erase(s); }
            catch (...) {}
        }
    }
}
