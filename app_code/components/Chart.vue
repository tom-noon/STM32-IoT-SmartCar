<!-- 条形图模块 -->
<template>
	<uni-card :title="myOption.title" :thumbnail="myOption.logo" >
		<l-echart ref="chart" @finished="init"></l-echart>
	</uni-card>
</template>
	
<script>
	import * as echarts from 'echarts'
	export default {
		props: {
			myOption:{
				type:Object
			}
		},
		data() {
			return {
				myChart:null,
				option:{
					tooltip: {
						trigger: 'axis'
					},
					grid: {
						left: 20,
						right: 20,
						bottom: 15,
						top: 40,
						containLabel: true
					},
					legend: {},
					// toolbox: {
					// 	show: true,
					// 	feature: {
					// 		dataZoom: {
					// 			yAxisIndex: 'none'
					// 		},
					// 		dataView: {
					// 			readOnly: false
					// 		},
					// 		magicType: {
					// 			type: ['line', 'bar']
					// 		},
					// 		restore: {},
					// 		saveAsImage: {}
					// 	}
					// },
					// dataZoom: [
					//     {
					//       type: 'inside',
					//       start: 0,
					//       end: 5
					//     },
					//     {
					//       start: 0,
					//       end: 10
					//     }
					//   ],
					xAxis: {
						type: 'category',
						boundaryGap: false,
						data:null,
					},
					yAxis: {
						type: 'value',
						axisLabel: {
							formatter: '',
						}
					},
					series: [{
							name: '',
							type: '',
							data: null,
							markPoint: {
								data: [{
										type: 'max',
										name: 'Max'
									},
									{
										type: 'min',
										name: 'Min'
									}
								]
							},
							markLine: {
								data: [{
									type: 'average',
									name: 'Avg'
								}]
							}
						}
					]
				}
			}
		},
		created() {
			this.option.xAxis.data=this.myOption.dataX
			this.option.yAxis.axisLabel.formatter='{value}'+this.myOption.danW,
			this.option.series[0].name=this.myOption.title.replace('折线图','')
			this.option.series[0].type=this.myOption.myType
			this.option.series[0].data=this.myOption.dataY
		},
		methods: {
			init() {
				this.$refs.chart.init(echarts, chart => {
					chart.setOption(this.option);
				});
			}
		},
		mounted() {
			this.$refs.chart.init(echarts, chart => {
				this.myChart=chart
				this.myChart.setOption(this.option)
			});
		},



		watch: {
			myOption(a,b){
				this.myChart.setOption(this.option)
			}
		}

	}
</script>

<style>
	.chartBox {
		width: 100%;
		height: 400px;
	}
</style>
