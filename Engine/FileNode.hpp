#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <type_traits>

struct Asset;
struct FNode;
struct Assets;
struct Project;

struct FNodeCreationParams {
    const Project* owner{ nullptr }; /* optional, if the file in question is owned by a project, set it */
    FNode* parent{ nullptr }; /* flexible, nullptr denotes root node, non-nullptr denotes non-root nodes */
    std::string name{}; /* mandatory, name of file/folder (extension is not part of the name) */
    std::string ext{}; /* optional, only applicable to files */
    std::string content{}; /* optional, only applicable to files */
    bool isDirectory{ false };
};

struct FNode {
    struct ChildrenList {
        template <typename T>
        struct Iterator {
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = value_type*;
            using reference = value_type&;
            using smart = const std::unique_ptr<std::remove_const_t<T>>;

            Iterator(smart* ptr) : ptr(ptr) {}

            reference operator*() const { return *(ptr->get()); }
            pointer operator->() { return ptr->get(); }
            Iterator<T>& operator++() { ptr++; return *this; }
            Iterator<T> operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
            friend bool operator==(const Iterator<T>& a, const Iterator<T>& b) = default;

        private:
            smart* ptr;
        };

        ChildrenList(std::vector<std::unique_ptr<FNode>>& children) noexcept;

        FNode& operator[](std::size_t idx);
        const FNode& operator[](std::size_t idx) const;

        Iterator<FNode> begin();
        Iterator<FNode> end();

        size_t size() const;

    private:
        std::vector<std::unique_ptr<FNode>>& children;
    };

    FNode(const FNodeCreationParams& params) noexcept;
    FNode(FNodeCreationParams&& params) noexcept;

    ~FNode() noexcept = default;
    FNode(const FNode& other) noexcept = delete;
    FNode(FNode&& other) noexcept = delete;
    FNode& operator=(const FNode& other) = delete;
    FNode& operator=(FNode&& other) noexcept = delete;
    bool operator==(const FNode& other) const noexcept;

    std::filesystem::path Path() const;
    std::filesystem::path AbsolutePath() const;
    std::filesystem::path FolderPath() const;

    std::string_view Name();
    const std::string& Name() const;
    void ChangeName(std::string_view name);
    void ChangeName(const std::string& name);
    void ChangeName(std::string&& name);

    std::string_view Extension();
    const std::string& Extension() const;
    void ChangeExtension(std::string_view extension);
    void ChangeExtension(const std::string& extension);
    void ChangeExtension(std::string&& extension);

    std::string_view FullName();
    const std::string& FullName() const;

    std::string_view Content() const;
    bool HasContent() const;
    bool ReadContent() const;
    void ModifyContent(std::string_view content);
    void ModifyContent(const std::string& content);
    void ModifyContent(std::string&& content);
    std::string DisposeContent() const;

    void MoveUnder(FNode& directory);
    void Delete();

    size_t Size() const;

    bool IsFile() const;
    bool IsDirectory() const;

    bool HasOwningProject() const;
    const Project* OwningProject() const;

    bool HasRootNode() const;
    FNode* RootNode() const;

    bool HasParentNode() const;
    FNode* ParentNode() const;

    ChildrenList Children();
    ChildrenList Children() const;

    static FNode HollowCopy(const FNode& other);

    FNode* CreateChildFile(FNodeCreationParams&& params);
    FNode* CreateChildFolder(FNodeCreationParams&& params);
    FNode* CreateChildFileIfNotExists(FNodeCreationParams&& params);
    FNode* CreateChildFolderIfNotExists(FNodeCreationParams&& params);
    bool DeleteChildNode(FNode& child);
    FNode& FindChild(const std::filesystem::path& path);

private:
    const Project* owner{ nullptr };
    FNode* parent{ nullptr };

    std::string name{};
    std::string ext{};
    std::string fullName{}; /* defined as name + ext (for example "hello" + ".txt") */
    mutable std::string content{}; /* files contents, can be disposed, can be stored... */

    bool isDirectory{ false };

    /* FNode guarantees references are always valid unless DeleteChildNode is called therefore, the unique_ptr's */
    /* are used here to prevent FNode objects from sliding around on deletions/reallocations of vector. */
    std::vector<std::unique_ptr<FNode>> children{};

    friend struct Asset;
    friend struct Assets;
};
