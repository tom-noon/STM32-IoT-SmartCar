
<!-- not use -->

<template> 
	<view>
		<uni-card :title="`车辆ID:${id}`" thumbnail="../../static/images/bike.png" class="bigCard">
			<view class="cu-form-group margin-top">
				<view class="title">开锁</view>
				<switch @change="changeLock" :class="lock?'checked':''" :checked="lock?true:false"></switch>
			</view> 
			<div class="cardList">
				<uni-card :title="card.title" v-for="card in cardList" style="width:30%;flex: none;">
					<img :src="card.url" :alt="card.title">
					<h1>{{card.value}}</h1>
				</uni-card>
				
			</div>
			<uni-card title="控制台" thumbnail="../../static/images/control.png">
				<view class="cu-form-group margin-top">
					<view class="title">鸣笛</view>
					<switch @change="changeBuzzer" :class="buzzer?'checked':''" :checked="buzzer?true:false"></switch>
				</view> 
				<view class="cu-form-group margin-top">
					<view class="title">转向</view>
					<view class="turn">
					<button>左转</button>
					<button>关闭</button>
					<button>右转</button>
					</view>
				</view> 
				
			</uni-card>
			<uni-card title="数据统计" thumbnail="../../static/images/chart.png">
				<My-Chart
				:myOption="{
					title:'电压折线图',
					logo:'../../static/images/volt.png',
					myType:'line',
					dataX:[1,23,32],
					dataY:[12,323,323]
				}"></My-Chart>
				
			</uni-card>
		</uni-card>
	</view>
</template>

<script>
	import request from '@/utils/request.js'
	import {
		buzzer,
		led,
		fan,
		lock
	} from '../../utils/mqttControl.js'
	import { mapState } from "vuex"
	export default {
		data() {
			return {
				lock:false,
				buzzer:null,
				id:null,
				carInfo:null,
				cardList:[
					{
						title:"电量",
						url:"../../static/images/power.png",
						value:''
					},
					{
						title:"位置",
						url:"../../static/images/point.png",
						value:''
					},
					{
						title:"状态",
						url:"../../static/images/bike.png",
						value:''
					},
					{
						title:"电压",
						url:"../../static/images/volt.png",
						value:''
					},
					{
						title:"电池温度",
						url:"../../static/images/tempCount.png",
						value:''
					}
				]
			}
		},
		onLoad(i) {
			this.id=i.id
			request({url:`bicycles/${i.id}`}).then(({data})=>{
				let info=data.data.attributes
				console.log(info);
				this.cardList[0].value=`${info.power*100}%`
				this.cardList[1].value=info.GPS,
				this.cardList[2].value=info.state?"运行中":"闲置中"
				this.$store.commit('lock/CHANGELOCK',info.state)
				this.lock=info.state
			})
		},
		computed:{
			...mapState(['isLock'])
		},
		methods: {
			changeLock(e) {
				this.lock=!this.lock
				this.cardList[2].value=this.lock?"运行中":"闲置中"
				lock(this.lock?0:1)
				this.$store.dispatch('lock/changeLock',this.id)
				// this.onLoad()
			},
			changeBuzzer(e){
				this.buzzer=!this.buzzer
				buzzer(this.buzzer?1:0)
			}
		}
	}
</script>

<style>
	.cardList{
		display: flex;
		flex-wrap: wrap;
		text-align: center;
		justify-content: space-evenly;
	}
	.turn{
		display: flex;
		width: 30%;
	}
	.turn button{
		/* display: block; */
		margin:0 10px                                                                                  ;
		width: 40%;
		justify-content: space-between;
	}
</style>
