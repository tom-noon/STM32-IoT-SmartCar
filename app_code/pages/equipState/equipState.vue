<template>
	<view class="equipState">
		<My-routeChange></My-routeChange>
		<uni-card title="设备状态" thumbnail="../../static/images/deviceControl.png">
			<view class="cardList">
				<uni-card v-for="equip in equipList" :title="equip.name" style="width:22%;flex: none;">
					<image :src="equip.icon" mode="widthFix" style="width:70%;"></image><br>
					{{equip.state}}
				</uni-card>
			</view>
		</uni-card>
		<view>
		</view>
		<My-Chart :myOption="{
			title:'温度折线图',
			logo:'../../static/images/tempCount.png',
			myType:'line',
			dataX:temp1List.createT,
			dataY:temp1List.data,
			danW:'℃'
		}" style="pointer-events:none" ></My-Chart>

		<My-Chart :myOption="{
			title:'湿度折线图',
			logo:'../../static/images/humid.png',
			myType:'line',
			dataX:humi1List.createT,
			dataY:humi1List.data,
			danW:'%'
		}" style="pointer-events:none" ></My-Chart>

	</view>

</template>

<script>
	import wind from '@/static/images/fan.png'
	import temp from '@/static/images/tempCount.png'
	import humi from '@/static/images/humid.png'
	import park from '@/static/images/park.png'
	import smoke from "@/static/images/smoke.png"
	import {
		mapState
	} from 'vuex'
	export default {
		data() {
			return {
				equipList: [{
						name: '温度',
						icon: temp,
						state: ""
					},
					{
						name: '湿度',
						icon: humi,
						state: ""
					},
					{
						name: '速度',
						icon: wind,
						state: ""
					}
				],

			}
		},
		created() {
			
		},
		computed: {
			...mapState('temp', ['temp1', 'temp1List']),
			...mapState('humi', ['humi1', 'humi1List']),
			...mapState('fan', ['spd']),
			lineSeries() {
				let series = [{
						name: "温度",
						data: this.temp1List.data,
					}
				]
				return series
			},
			lineX() {
				let cate = this.temp1List.createT
				return cate
			},
			columnSeries() {
				let series = [{
						name: "检测温度大于25",
						data: [this.biggerTemp1]
					},
					{
						name: "检测温度小于25",
						data: [this.lessTemp1]
					},
				]
				return series
			},
			columnX() {
				return []
			},
		},
		watch: {
				
			temp1:{
				deep:true,
				immediate:true,
				handler(a, b) 
				{
					if(a===null)
					this.equipList[0].state = b
					else
					this.equipList[0].state = a
				}
			},
			humi1:{
				deep:true,
				immediate:true,
				handler(a, b) 
				{
					if(a===null)
					this.equipList[1].state = b
					else
					this.equipList[1].state = a
				}
			},
			spd:{
				deep:true,
				immediate:true,
				handler(a, b) 
				{
					if(a===null)
					this.equipList[2].state = b
					else
					this.equipList[2].state = a
				}
			},
		}
	}
</script>

<style>
	.cardList {
		display: flex;
		flex-wrap: wrap;
		justify-content: center;
		text-align: center;
	}
</style>
