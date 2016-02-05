/*
 * Created by Will Pryor at the Autonomous Research Collaboration Lab at Worcester Polytechnic Institute
 * Using framework from the pcl_ros package at http://wiki.ros.org/pcl_ros
 */

#ifndef SAC_SEGMENT_AND_FIT_H
#define SAC_SEGMENT_AND_FIT_H

#include <pcl_ros/pcl_nodelet.h>

// PCL includes
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/search/pcl_search.h>
#include <pcl/filters/project_inliers.h>

// Dynamic reconfigure
#include <dynamic_reconfigure/server.h>
#include <surface_filters/SACConfig.h>

#include <surfaces/PointClusters.h>
#include <surfaces/PointClusters_Serialization.h>

namespace surface_filters {
    namespace sync_policies = message_filters::sync_policies;

    /** \brief @b RegionGrowingSegmentation represents a nodelet using the RegionGrowingSegmentation implementation.
    * \author Will Pryor
    */
    class SACSegmentAndFit : public pcl_ros::PCLNodelet {
        // Point types
        typedef pcl::PointXYZ PointIn; // It actually will have RGB info too, but pcl_ros freaks out if you tell it that
        typedef pcl::PointNormal NormalIn;
        typedef pcl::PointXYZRGB PointOut;

        // Point Cloud types
        typedef pcl::PointCloud<PointIn> PointCloudIn;
        typedef pcl::PointCloud<NormalIn> NormalCloudIn;
        typedef pcl::PointCloud<PointOut> PointCloudOut;

        typedef pcl::PointIndices PointIndices;

        typedef surfaces::PointClusters PointClusters;

        // Message synchronizer types
        template<typename ...SubscribedTypes>
        using ExactTimeSynchronizer = message_filters::Synchronizer<sync_policies::ExactTime<SubscribedTypes...> >;
        template<typename ...SubscribedTypes>
        using ApproximateTimeSynchronizer = message_filters::Synchronizer<sync_policies::ApproximateTime<SubscribedTypes...> >;


    protected:
        /** \brief Set the model type */
        int model_type_;

        /** \brief Set the SAC method */
        int method_type_;

        /** \brief Set the threshold for distance to the model */
        double dist_threshold_;

        /** \brief Set the maximum number of iterations before giving up */
        int max_iterations_;

        /** \brief Set the probability of choosing at least one sample free from outliers */
        double probability_;

        /** \brief Set to true if a coefficient refinement is required */
        bool optimize_coefficients_;

        /** \brief Set the minimum allowable radius for the model */
        double radius_min_;

        /** \brief Set the maximum allowable radius for the model */
        double radius_max_;

        /** \brief Set the maximum allowed difference between the model normal and the given axis in radians */
        double epsilon_angle_;

        /** \brief Set to true to use normals */
        bool use_normals_;

        /** \brief Minimum number of points in a cluster */
        unsigned int min_points_;

        /** \brief Pointer to a dynamic reconfigure service. */
        boost::shared_ptr<dynamic_reconfigure::Server<SACConfig> > srv_;

        /** \brief Dynamic reconfigure callback
          * \param config the config object
          * \param level the dynamic reconfigure level
          */
        void config_callback(SACConfig &config, uint32_t level);

    private:
        /** \brief Nodelet initialization routine. */
        virtual void onInit();

        /** \brief Input point cloud callback.
          * \param cloud the pointer to the input point cloud
          * \param indices the pointer to the input point cloud indices
          */
        void synchronized_input_callback(const PointCloudIn::ConstPtr &cloud, const NormalCloudIn::ConstPtr &normals,
                                         const PointClusters::ConstPtr &input_clusters);


    private:
        /** \brief The PCL implementation used. */
        pcl::SACSegmentation<PointIn> impl_;

        /** \brief The concave hull implementation, used for visualization only */
        pcl::ProjectInliers<PointIn> project_;

        /** \brief The clusters PointCloud publisher. */
        ros::Publisher pub_clusters_;

        /** \brief Synchronized input, and indices.*/
        boost::shared_ptr<ExactTimeSynchronizer<PointCloudIn, NormalCloudIn, PointClusters> > sync_input_normals_clusters_e_;
        boost::shared_ptr<ApproximateTimeSynchronizer<PointCloudIn, NormalCloudIn, PointClusters> > sync_input_normals_clusters_a_;
        boost::shared_ptr<ExactTimeSynchronizer<PointCloudIn, PointClusters> > sync_input_clusters_e_;
        boost::shared_ptr<ApproximateTimeSynchronizer<PointCloudIn, PointClusters> > sync_input_clusters_a_;

        /** \brief The message filter subscriber for PointCloud2. */
        message_filters::Subscriber<NormalCloudIn> sub_normals_filter_;
        message_filters::Subscriber<PointClusters> sub_clusters_filter_;

    public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    };
}

#endif // RANSAC_SEGMENT_AND_FIT_H
