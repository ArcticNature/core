// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_MODEL_REPOSITORY_H_
#define CORE_MODEL_REPOSITORY_H_

#include <istream>
#include <memory>
#include <string>


namespace sf {
namespace core {
namespace model {

  //! Operates on a specific version of the repositry.
  class RepositoryVersion;
  typedef std::shared_ptr<RepositoryVersion> RepositoryVersionRef;

  //! Reference counting on std::istream pointers.
  typedef std::shared_ptr<std::istream> IStreamRef;

  //! Base definition of a repository.
  /*!
   * In order to be repository system agnostic all repositories are
   * accessed through this generic interface.
   */
  class Repository {
   public:
    virtual ~Repository();

    //! Returns an object describing the latest version in the repository.
    virtual RepositoryVersionRef latest() = 0;

    //! Returns an object describing the requested version of the repository.
    virtual RepositoryVersionRef version(std::string version) = 0;

    //! Returns an object describing the requested version of the repository.
    /*!
     * The described version is either the latest version
     * or the the requested one, if it exists.
     * 
     * The latest version is used if the requested version is:
     *   * The empty string.
     *   * The string '<latest>'
     */
    RepositoryVersionRef lookup(std::string version);

    //! Returns a unique identifier for the requested version.
    /*!
     * Since some version control systems allows aliases and short names
     * it is often useful to resolve a version to a normalised version.
     */
    virtual std::string resolveVersion(const std::string version) = 0;

    //! Ensures the requested version exists.
    /*!
     * The version is identified by the user when the request for
     * configuration is made (either command line at start or as a command).
     * 
     * When implementing repositories keep in mind that the version
     * "<latest>" is reserved to identify the latest version that can be
     * fetched and not a specific version.
     */
    virtual void verifyVersion(const std::string version) = 0;
  };
  typedef std::shared_ptr<Repository> RepositoryRef;

  //! Operates on a specific version of the repositry.
  class RepositoryVersion {
   public:
    virtual ~RepositoryVersion();

    //! Checks if a path exists in the repository.
    virtual bool exists(std::string) = 0;

    //! Finds the file containing the definition of a service.
    /*!
     * Resolves a service identifier to the closest definition path.
     * Service IDs are dot separated directory names.
     * This method looks for the most complete name that can be found.
     * 
     * For example for service "tests.internal.web-service" the following
     * paths would be checked, in order, and the first existing path is
     * returned.
     *   - tests/internals/web-service.service
     *   - tests/internals.service
     *   - tests.service
     * 
     * \param service The service identifier to lookup.
     * \param prefix  The path to the directory storing the definitions.
     * \returns The path to the file with the definition.
     */
    virtual std::string findDefinitionFile(
        const std::string service, const std::string prefix = "services"
    );

    //! Expose a file as a managed pointer to an input stream.
    virtual IStreamRef streamFile(const std::string path) = 0;
  };

}  // namespace model
}  // namespace core
}  // namespace sf

#endif  // CORE_MODEL_REPOSITORY_H_
