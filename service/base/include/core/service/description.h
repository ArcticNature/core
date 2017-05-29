// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_SERVICE_DESCRIPTION_H_
#define CORE_SERVICE_DESCRIPTION_H_

#include <json.hpp>

#include <map>
#include <set>
#include <string>


namespace sf {
namespace core {
namespace service {

  // Forward declarations.
  class ServiceDescriptionBuilder;

  //! Detailed description of a service instance.
  /*!
   * Stores the description of a service, what it should run and how.
   * For options that change based on atributes (connector, ...)
   * use JSON objects to store the data.
   *
   * Service definitions are used to start and describe instances.
   * Additional information (such as instance id, process location, ...)
   * are outside the scope of this description and are stored in a
   * service instance object for the specific instance.
   */
  class ServiceDescription {
   public:
    //! Returns a service builder instance.
    static ServiceDescriptionBuilder Build();

    //! Create a service description instance from a json object.
    static ServiceDescription Load(nlohmann::json object);

   public:
    //! Returns an hash of the service description.
    /*!
     * The hash is computed by combining the service id
     * with the configuration version.
     */
    std::size_t hash() const;

    //! Convert this description to a json object.
    nlohmann::json jsonify() const;

    // *** Basic configuration. *** //
    //! Version of the configuration where this definition is defined.
    std::string configVersion() const;

    //! Connector used to interact with the service.
    std::string connector() const;

    //! Retrive a named port.
    uint32_t port(std::string name) const;

    //! Ports used by the process.
    /*!
     * Ports are named for used during configuration.
     * Having named ports allows references from other services and
     * from service discovery.
     */
    std::map<std::string, uint32_t> ports() const;

    //! The ID of the service described by this instance.
    std::string serviceId() const;

    // *** Custom options used by other components. *** //
    //! Connector specific configuration.
    nlohmann::json connectorConfig() const;

    // *** Execution environment. *** //
    //! Environment variables mappings.
    std::map<std::string, std::string> environment() const;

    //! Name of the group to run as.
    std::string group() const;

    //! Name of the user to run as.
    std::string user() const;

    // Allow derived classes with destructors.
    virtual ~ServiceDescription() = default;

   protected:
    std::string config_version_;
    std::string connector_;
    nlohmann::json connector_config_;
    std::string service_id_;

    std::map<std::string, std::string> environment_;
    std::map<std::string, uint32_t> ports_;

    std::string group_;
    std::string user_;

    // Protect the constructor for factory methods only.
    ServiceDescription() = default;

    // Allow the builder to set attributes.
    friend ServiceDescriptionBuilder;
  };


  //! Builder class for a service description.
  class ServiceDescriptionBuilder {
   public:
    //! Validate required attributes and builds a ServiceDescription instance.
    ServiceDescription build();

    ServiceDescriptionBuilder configVersion(std::string config_version);
    ServiceDescriptionBuilder connector(std::string connector);
    ServiceDescriptionBuilder connectorConfig(nlohmann::json config);
    ServiceDescriptionBuilder environment(std::string key, std::string value);
    ServiceDescriptionBuilder group(std::string group);
    ServiceDescriptionBuilder port(std::string name, uint32_t port);
    ServiceDescriptionBuilder serviceId(std::string service_id);
    ServiceDescriptionBuilder user(std::string user);

   protected:
    ServiceDescription description_;
    std::set<uint32_t> used_ports_;
  };

}  // namespace service
}  // namespace core
}  // namespace sf

#endif  // CORE_SERVICE_DESCRIPTION_H_
