class CustomTransform : public TransformationComponent {
public:
    CustomTransform(const glm::mat4& mat) : matrix(mat) {}

    void apply(glm::mat4& mat) const override {
        mat = mat * matrix;
    }

private:
    glm::mat4 matrix;
};
